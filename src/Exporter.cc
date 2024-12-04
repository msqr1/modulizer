#include "Exporter.hpp"
#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "../3rdParty/Generator.hpp"
#include <string>
#include <string_view>
#include <stack>
#include <vector>
#include <ranges>

void matchBrace(size_t& pos, std::string_view str) {
  int nest{1};
  while(nest) {
    switch(str[++pos]) {
    case '{':
      nest++;
      break;
    case '}':
      nest--;
    }
  }
}

// PCRE2 for non-matching capture groups, and
// std::string::find* for not found return a -1 size_t  
size_t notFound{static_cast<size_t>(-1)};

bool onlyWhitespace(std::string_view str, size_t start, size_t end) {
  return str.substr(start, end - start).find_first_not_of(" \n\t") == notFound;
}

struct Export {
  size_t start;
  size_t end;
};

struct DeclScope {
  size_t declStart;
  size_t start;

  // For searching
  size_t startOffset;
  size_t end;
  DeclScope(size_t declStart, size_t start, size_t startOffset, size_t end): 
    declStart{declStart}, start{start}, startOffset{startOffset}, end{end} {}
  DeclScope(): startOffset{} {};
};

// Exports for everything but static unions and anonymous namespace 
// (sorry I can't think of a better name for this)
cppcoro::generator<Export> getExports1(std::string_view content) {

  // Matches namespace or UNNAMED union. Capture the "s" in static to try testing if the
  // union is static or not. Capture the "u" to see if it is a namespace or union.
  re::Pattern pat{R"((?:(?:(s)tatic|inline)\s++)?(?:(u)nion\s*|namespace([^\{]*))\{)"};
  std::stack<DeclScope> stack;
  stack.emplace(0, 0, 0, content.size());
  DeclScope self;
  std::string_view toMatch;
  std::optional<re::Captures> maybeCaptures;
  re::Captures captures;

  while(!stack.empty()) {
    DeclScope& parent = stack.top();
    toMatch = content.substr(0, parent.end);
    if((maybeCaptures = pat.match(toMatch, parent.start + parent.startOffset))) {
      captures = *maybeCaptures;
      self.declStart = captures[0].start;
      self.start = self.end = captures[0].end;
      matchBrace(self.end, toMatch);
      
      // If this is a union (seeing if the captured "u" is there)
      bool isUnion{captures[2].start != notFound};
      bool isStaticUnion{};
      if(isUnion) {
        size_t unionDeclEnd{toMatch.find(';', self.end + 1)};

        // Check if "static" is in the front (seeing if the captured "s" is there) or if
        // it is after the end
        isStaticUnion = captures[1].start != notFound || 
        toMatch.substr(self.end + 1, unionDeclEnd - self.end).contains("static");
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
          co_yield {parent.start, self.declStart};
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
        co_yield {parent.start, parent.end};
      stack.pop();
    }
  }
}

// Exports for everything but static variables/functions (no good name either)
cppcoro::generator<Export> getExports2(std::string_view content, Export export1) {
  co_yield export1;
}
void addExports(std::string& content, const Opts& opts) {
  std::vector<Export> exports;
  for(Export exp1 : getExports1(content)) {
    for(Export exp : getExports2(content, exp1)) {
      exports.emplace_back(exp);
    }
  }
  for(auto [start, end] : std::views::reverse(exports)) {
    content.insert(end, opts.closeExport).insert(start, opts.openExport);
  }
}