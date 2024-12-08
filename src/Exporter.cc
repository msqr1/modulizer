#include "Exporter.hpp"
#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "../3rdParty/Generator.hpp"
#include <string>
#include <string_view>
#include <stack>
#include <vector>
#include <ranges>

// Generic template for matching braces, parentheses, ...
template <char open, char close> void balance(size_t& pos, std::string_view str) {
  int nest{1};
  while(nest) {
    switch(str[++pos]) {
    case open:
      nest++;
      break;
    case close:
      nest--;
    }
  }
}

auto balanceBrace = balance<'{','}'>;

// PCRE2 for non-matching capture groups, and
// std::string::find* for not found return a -1 size_t  
size_t notFound{static_cast<size_t>(-1)};

bool onlyWhitespace(std::string_view str, size_t start, size_t end) {
  return str.substr(start, end - start).find_first_not_of(" \n\t") == notFound;
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
// (sorry I can't think of a better name for this)
cppcoro::generator<const Export&> getExports1(std::string_view content) {
  Export rtn;
  
  // Matches namespace or UNNAMED union. Capture the "s" in static to try testing if the
  // union is static or not. Capture the "u" to see if it is a namespace or union.
  re::Pattern pat{R"((?:(?:(s)tatic|inline)\s++)?(?:namespace([^\{]*+)|(u)nion\s*+)\{)"};
  std::optional<re::Captures> maybeCaptures;
  re::Captures captures;
  std::stack<NSorUnion> stack;
  stack.emplace(0, 0, 0, content.size());
  NSorUnion self;
  std::string_view toMatch;
  while(!stack.empty()) {
    NSorUnion& parent = stack.top();
    toMatch = content.substr(0, parent.end);
    if((maybeCaptures = pat.match(toMatch, parent.start + parent.startOffset))) {
      captures = *maybeCaptures;
      self.declStart = captures[0].start;
      self.start = self.end = captures[0].end;
      balanceBrace(self.end, toMatch);
      
      // If this is a union (seeing if the captured "u" is there)
      bool isUnion{captures[2].start != notFound};
      bool isStaticUnion{};
      if(isUnion) {
        size_t unionDeclEnd{toMatch.find(';', self.end + 1)};

        // Check if "static" is in the front (seeing if the captured "s" is there) or if
        // it is after the end
        isStaticUnion = captures[1].start != notFound || 
        toMatch.substr(self.end + 1, unionDeclEnd - self.end - 1).contains("static");
        self.end = unionDeclEnd;
      }
      re::Capture NSCapture{captures[3]};
      bool isUnnamedNS{!isUnion && onlyWhitespace(toMatch, NSCapture.start, NSCapture.end)};
      
      // Unnamed union variable (union {...} name;)
      if(isUnion && !isStaticUnion) {
        parent.startOffset = self.end - parent.start + 1;
      }
      
      // Namespace or static union
      else {
        if(!onlyWhitespace(toMatch, parent.start, self.declStart)) 
          co_yield rtn.set(parent.start, self.declStart);
        parent.start = self.end + 1;
        parent.startOffset = 0;

        // Named namespace, if we only do !isUnnamedNS, a union will also not 
        // count as a namespace, so isUnnamedNS will be false and this runs
        if(!(isUnion || isUnnamedNS)) stack.emplace(self);
      }
    }

    // No match (no more to export)
    else {
      if(!onlyWhitespace(toMatch, parent.start, parent.end))
        co_yield rtn.set(parent.start, parent.end);
      stack.pop();
    }
  }
}

// Exports for everything but static variables/functions (no good name either)
cppcoro::generator<const Export&> getExports2(std::string_view content, const Export& export1) {
  Export rtn;
  content.remove_suffix(content.size() - export1.end);
  
  // Regex to find classes and structs to avoid
  re::Pattern typePat{R"((?:class|struct)[^{]{2,}{)"};
  std::optional<re::Captures> maybeCaptures;
  re::Capture classDecl;
  size_t startOffset{export1.start};
  while((maybeCaptures = typePat.match(content, startOffset))) {
    classDecl = (*maybeCaptures)[0];
    // Process static symbols from start to classDecl.start
    log("e: {}", content.substr(startOffset));
    startOffset = classDecl.end;
    balanceBrace(startOffset, content);
    startOffset = content.find(';', startOffset + 1) + 1;
  }
  // Process static symbols from start to content's end
  co_yield export1;
} 
void addExports(std::string& content, const Opts& opts) {

  //std::vector<Export> exports;
  for(const Export& exp1 : getExports1(content)) {
    for(const Export& exp : getExports2(content, exp1)) {
      
    }
  }
  /*for(const auto& [start, end] : std::views::reverse(exports)) {
    content.insert(end, opts.closeExport).insert(start, opts.openExport);
  }*/
}