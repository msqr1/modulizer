#include "Exporter.hpp"
#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "Regex.hpp"
#include "../3rdParty/Generator.hpp"
#include <string>
#include <string_view>
#include <stack>

namespace {

constexpr std::string_view whitespaces{" \n\t"};

void balanceBrace(std::string_view str, size_t& pos) {
  int nest{1};
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

// Usually checked before co_yield'ing an export
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

// Namespaces are for grouping and anti-pollution purpose

namespace unionAndNSExport {

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
cppcoro::generator<const Export&> get(std::string_view content, size_t lastImportEnd) {
  Export rtn;
  
  // Matches namespace or UNNAMED union. Capture the "s" in static to try testing if the
  // union is static or not. Capture the "u" to see if it is a namespace or union.
  re::Pattern pat{R"((?:(?:(s)tatic|inline)\s++)?(?:namespace([^\{]*+)|(u)nion\s*+)\{)"};
  std::optional<re::Captures> maybeCaptures;
  re::Captures captures;
  re::Capture NSCapture;
  std::stack<NSorUnion> stack;
  stack.emplace(lastImportEnd, lastImportEnd, 0, content.size());
  NSorUnion self;
  std::string_view toMatch;
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

        // Check if "static" is in the front (seeing if the captured "s" is there) or 
        // after the union's end
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

        // Named namespace
        if(!(isUnion || isUnnamedNS)) stack.emplace(self);
      }
    } 
    else {
      if(!onlyWhitespace(toMatch, parent.start, parent.end)) {
        co_yield rtn.set(parent.start, parent.end);
      }
      stack.pop();
    }
  } while(!stack.empty());
}

} // namespace unionAndNSExport

namespace staticSymbolExport {

// Regex to find classes and structs to avoid
const re::Pattern typePat{R"((?:class|struct)[^{]{2,}{)"};

// Regex to find static symbols
const re::Pattern pat{R"(static[^;{]+[;{])"};

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
cppcoro::generator<const Export&> get(std::string_view content, 
const Export& exp1) {
  Export rtn;
  content.remove_suffix(content.size() - exp1.end);
  std::optional<re::Captures> maybeCaptures;
  std::optional<size_t> maybeTemplate;
  size_t processed{exp1.start};
  size_t lastExportEnd{exp1.start};
  auto exportStatics = [&](std::string_view subject) -> 
    cppcoro::generator<const Export&> {
    while((maybeCaptures = pat.match(subject, processed))) {
      auto [start, end]{(*maybeCaptures)[0]};
      char endIsBrace{subject[end - 1] == '{'};
      bool isFn{endIsBrace && subject.substr(start).rfind(')', end) != notFound};
      if((maybeTemplate = getTemplate(subject, start))) start = *maybeTemplate;
      if(!onlyWhitespace(subject, lastExportEnd, start)) {
        co_yield rtn.set(lastExportEnd, start);
      }
      if(endIsBrace) {
        balanceBrace(subject, end);
        if(!isFn) end = subject.find(';', end) + 1;
      }
      lastExportEnd = processed = end;
    }
  };
  {
    std::string_view toMatch;
    re::Capture classDecl; 
    while((maybeCaptures = typePat.match(content, processed))) {
      classDecl = (*maybeCaptures)[0];
      toMatch = content.substr(0, classDecl.start);
      for(const Export& exp : exportStatics(toMatch)) {
        co_yield exp;
      }
      processed = classDecl.end;
      balanceBrace(content, processed);
      processed = content.find(';', processed) + 1;
    }
  }
  for(const Export& exp : exportStatics(content)) {
    co_yield exp;
  }
  if(!onlyWhitespace(content, processed, exp1.end)) {
    co_yield rtn.set(processed, exp1.end);
  }
}

} // namespace staticSymbolExport

} // namespace

void addExports(std::string& content, size_t lastImportEnd, const Opts& opts) {
  logIfVerbose("Acquiring exports...");
  
  // We don't insert the exports while scanning because that will severely
  // increase the complexity of calculating offset (notice how these are nested)
  // Besides, Exports are just number pairs, easily stored
  std::stack<Export> exports;
  for(const Export& exp1 : unionAndNSExport::get(content, lastImportEnd)) {
    for(const Export& exp : staticSymbolExport::get(content, exp1)) {
      exports.push(exp);
    }
  }
  logIfVerbose("Inserting exports...");
  do {
    const auto [start, end]{exports.top()};
    content.insert(end, opts.closeExport).insert(start, opts.openExport);
    exports.pop();
  } while(!exports.empty());
}