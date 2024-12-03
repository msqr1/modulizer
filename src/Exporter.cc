#include "Exporter.hpp"
#include "Base.hpp"
#include "ArgProcessor.hpp"
#include <string>
#include <string_view>
#include <stack>
#include "../3rdParty/Generator.hpp"

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

struct Export {
  size_t start;
  size_t end;
};

struct DeclScope {
  size_t declStart;
  size_t start;

  // For searching
  size_t startOffset{};
  size_t end;
};

// Exports for everything but static unions and anonymous namespace (sorry I can't think of a better name for this)
void getExports1(std::string_view content) {

  // Matches namespace or UNNAMED union. Capture the "s" in static to try testing if the union is static or not. Capture the "u" to see if it is a namespace or union.
  re::Pattern pat{R"((?:(?:(s)tatic|inline)\s++)?(?:(u)nion\s*|namespace([^\{]*))\{)"};
  std::stack<DeclScope> stack;
  stack.emplace(0, content.size());
  DeclScope self;
  std::string_view toMatch;
  std::optional<re::Captures> maybeCaptures;
  re::Captures captures;

  // PCRE2 for non-matching capture groups, and
  // std::string::find* for not found return a -1 size_t  
  size_t notFound{static_cast<size_t>(-1)};
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
      bool isStaticUnion;
      if(isUnion) {
        size_t unionDeclEnd{toMatch.find(';', self.end + 1)};

        // Check if "static" is in the front (seeing if the captured "s" is there)
        isStaticUnion = captures[1].start != notFound || toMatch.substr(self.end + 1, unionDeclEnd).contains("static");
        self.end = unionDeclEnd;
      }
      re::Capture NSCapture{captures[3]};
      bool isUnnamedNS{!isUnion &&

        // Check if the namespace name consists of only whitespace
        toMatch.substr(NSCapture.start, NSCapture.end - NSCapture.start).find_first_not_of(" \n\t") == notFound
      };
      
      // Unnamed union variable (union {...} var;)
      if(isUnion && !isStaticUnion) {
        parent.startOffset = self.end - parent.start + 1;
      }
      
      // Namespace or static union
      else {
        log("1. Exporting: {} {}", parent.start, self.declStart - parent.start);
        parent.start = self.end + 1;
        parent.startOffset = 0;

        // Named namespace, if we only do !isUnnamedNS, a union will also not count as a namespace, so isUnnamedNS will be false and condition execute
        if(!(isUnion || isUnnamedNS)) {
          stack.emplace(self);
        }
      }
    }

    // No match (no more to export)
    else {
      // TODO: Fix end index behavior
      log("2. Exporting: {} {}", parent.start, parent.end - parent.start);
      stack.pop();
    }
  }
}

void addExports(std::string& content, const Opts& opts) {
  getExports1(content);
}