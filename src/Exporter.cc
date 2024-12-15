#include "Exporter.hpp"
#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "../3rdParty/Generator.hpp"
#include <string>
#include <string_view>
#include <stack>

// Generic template for matching braces, parentheses, ...
// Set pos to ONE PAST the matching brace (just the nature of the algorithm)
void balanceBrace(std::string_view str, size_t& pos) {
  int nest{1};

  // 1st iteration always run, use do-while
  do {
    switch(str[pos]) {
    case '{':
      nest++;
      break;
    case '}':
      nest--;
    }
    ++pos;
  } while(nest); 
}

// PCRE2 for non-matching capture groups, and
// std::string::find* for not found return a -1 size_t (std::string::npos)
size_t notFound{static_cast<size_t>(-1)};
std::string_view whitespaces{" \n\t"};

bool onlyWhitespace(std::string_view str, size_t start, size_t end) {
  return str.substr(start, end - start).find_first_not_of(whitespaces) == notFound;
}

struct Export {
  size_t start;
  size_t end;

  const Export& set(size_t start, size_t end) {
    this->start = start;
    this->end = end;
    return *this;
  }
};

struct NSorUnion {
  size_t declStart;
  size_t start;

  // For searching
  size_t startOffset;
  size_t end;

  NSorUnion(size_t declStart, size_t start, size_t startOffset, size_t end): 
    declStart{declStart}, start{start}, startOffset{startOffset}, end{end} {}
  NSorUnion(): startOffset{} {};
};

// Exports for everything but static unions and anonymous namespace 
cppcoro::generator<const Export&> unionAndNSExports(std::string_view content) {
  Export rtn;
  
  // Matches namespace or UNNAMED union. Capture the "s" in static to try testing if the
  // union is static or not. Capture the "u" to see if it is a namespace or union.
  re::Pattern pat{R"((?:(?:(s)tatic|inline)\s++)?(?:namespace([^\{]*+)|(u)nion\s*+)\{)"};
  std::optional<re::Captures> maybeCaptures;
  re::Captures captures;
  re::Capture NSCapture;
  std::stack<NSorUnion> stack;
  stack.emplace(0, 0, 0, content.size());
  NSorUnion self;
  std::string_view toMatch;

  // 1st iteration always run, use do-while
  do {
    NSorUnion& parent = stack.top();
    toMatch = content.substr(0, parent.end);
    if((maybeCaptures = pat.match(toMatch, parent.start + parent.startOffset))) {
      captures = *maybeCaptures;
      self.declStart = captures[0].start;
      self.start = self.end = captures[0].end;
      balanceBrace(toMatch, self.end);
      
      // If this is a union (seeing if the captured "u" is there)
      bool isUnion{captures[3].start != notFound};
      bool isStaticUnion{};
      if(isUnion) {
        size_t declSemicolon{toMatch.find(';', self.end)};

        // Check if "static" is in the front (seeing if the captured "s" is there) or if
        // "static" is between the semicolon and the closing brace
        isStaticUnion = captures[1].start != notFound || 
        toMatch.substr(self.end, declSemicolon - self.end).contains("static");
        self.end = declSemicolon + 1;
      }
      NSCapture = captures[2];
      bool isUnnamedNS{!isUnion && onlyWhitespace(toMatch, NSCapture.start, NSCapture.end)};
      
      // Unnamed union variable (union {...} name;)
      if(isUnion && !isStaticUnion) parent.startOffset = self.end - parent.start;

      // Namespace or static union
      else {
        if(!onlyWhitespace(toMatch, parent.start, self.declStart)) {
          co_yield rtn.set(parent.start, self.declStart);
        }
        parent.start = self.end;
        parent.startOffset = 0;

        // self.end is currently one past the closing brace (what balanceBrace do).
        // Since it will be the match limit for next iteration, we can't include
        // the brace itself.
        self.end--;

        // Named namespace, if we only do !isUnnamedNS, a union will also not 
        // count as a namespace, so isUnnamedNS will be false and this runs
        if(!(isUnion || isUnnamedNS)) stack.emplace(self);
      }
    } 

    // No match (no more to export)
    else {
      if(!onlyWhitespace(toMatch, parent.start, parent.end)) {
        co_yield rtn.set(parent.start, parent.end);
      }
      stack.pop();
    }
  } while(!stack.empty());
}

// Get the start of a potential template declaration (the 't' in template). 
// This is used when we want to backscan a variable/function declaration 
// to get he preceding template<...> part if it has one
std::optional<size_t> getTemplate(std::string_view str, size_t idx) {
  idx--;
  for(;idx >= 0; idx--) {
    char current{str[idx]};
    bool isWhitespace{};
    for(char c : whitespaces) {
      if(c == current) {
        isWhitespace = true;
        break;
      }
    }
    if(!isWhitespace) {
      if(current != '>') return std::nullopt;
      else break;
    }
  }

  // In case it is all whitespace to str's start
  if(idx == 0) return std::nullopt;
  int nest{1};
  do {
    switch(str[--idx]) {
    case '>':
      nest++;
      break;
    case '<':
      nest--;
    }
  } while(nest);
  return str.rfind("template", --idx);
}

// Export for everything but static symbols
// The general idea is we scan for classes to avoid them, then we scan for
// static symbols in between those classes, and export code in the middle
// of the subsequent static symbols (a class could be in there too)
cppcoro::generator<const Export&> staticSymbolExports(std::string_view content, 
const Export& exp1) {
  Export rtn;
  content.remove_suffix(content.size() - exp1.end);
  std::string_view toMatch;

  // Regex to find static symbols
  // Static because this function is called many times, else it will segfault
  // and I don't know why.
  static re::Pattern pat{R"(static[^;{]+[;{])"};
  std::optional<re::Captures> maybeCaptures;
  std::optional<size_t> maybeTemplate;
  size_t processed{exp1.start};
  size_t lastExportEnd{exp1.start};
  {
    // Regex to find classes and structs to avoid
    // Static because this function is called many times, else it will segfault
    // and I don't know why.
    static re::Pattern typePat{R"((?:class|struct)[^{]{2,}{)"};
    re::Capture classDecl;
    
    while((maybeCaptures = typePat.match(content, processed))) {
      classDecl = (*maybeCaptures)[0];
      toMatch = content.substr(0, classDecl.start);
      while((maybeCaptures = pat.match(toMatch, processed))) {
        auto [start, end]{(*maybeCaptures)[0]};
        char endIsBrace{toMatch[end - 1] == '{'};
        bool isFn{endIsBrace && toMatch.substr(start).rfind(')', end) != notFound};
        if((maybeTemplate = getTemplate(toMatch, start))) start = *maybeTemplate;
        if(!onlyWhitespace(toMatch, lastExportEnd, start)) {
          co_yield rtn.set(lastExportEnd, start);
        }
        if(endIsBrace) {
          balanceBrace(toMatch, end);

          // A brace-initialized variable, find its ending semicolon
          if(!isFn) end = toMatch.find(';', end) + 1;
        }
        lastExportEnd = processed = end;
      }
      processed = classDecl.end;
      balanceBrace(content, processed);
      processed = content.find(';', processed) + 1;
    }
  }
  while((maybeCaptures = pat.match(content, processed))) {
    auto [start, end]{(*maybeCaptures)[0]};
    char endIsBrace{content[end - 1] == '{'};
    bool isFn{endIsBrace && content.substr(start).rfind(')', end) != notFound};
    if((maybeTemplate = getTemplate(content, start))) start = *maybeTemplate;
    if(!onlyWhitespace(content, lastExportEnd, start)) {
      co_yield rtn.set(lastExportEnd, start);
    }
    if(endIsBrace) {
      balanceBrace(content, end);

      // A brace-initialized variable, find its ending semicolon
      if(!isFn) end = content.find(';', end) + 1;
    }
    lastExportEnd = processed = end;
  }
  if(!onlyWhitespace(content, processed, exp1.end)) {
    co_yield rtn.set(processed, exp1.end);
  }
} 

void addExports(std::string& content, const Opts& opts) {
  std::stack<Export> exports;
  for(const Export& exp1 : unionAndNSExports(content)) {
    for(const Export& exp : staticSymbolExports(content, exp1)) exports.push(exp);
  }

  // 1st iteration always run, use do-while
  do {
    auto [start, end]{exports.top()};
    content.insert(end, opts.closeExport).insert(start, opts.openExport);
    exports.pop();
  } while(!exports.empty());
}