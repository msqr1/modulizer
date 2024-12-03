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
    switch(str[pos++]) {
    case '{':
      nest++;
      break;
    case '}':
      nest--;
    }
  }
}

struct Scope {
  size_t start;
  size_t end;
  Scope(size_t start, size_t end): start{start}, end{end} {};
  Scope() {};

  // For convenience
  Scope& operator=(re::Capture capture) {
    start = capture.start;
    end = capture.end;
    return *this;
  }
  
};

// Export have a start and an end, just like a scope block, so I just alias it.
using Export = Scope;

// Exports for everything but static unions and anonymous namespace (sorry I can't think of a better name for this)
void getExports1(std::string_view content) {

  // Matches namespace or UNNAMED union. Capture the "s" in static to try testing if the union is static or not. Capture the "u" to see if it is a namespace or union.
  re::Pattern pat{R"((?:(?:(s)tatic|inline)\s++)?(?:(u)nion\s*|namespace([^\{]*))\{)"};
  std::stack<Scope> stack;
  stack.emplace(0, content.size());
  Scope parent;
  Scope self;
  std::string_view toMatch;
  std::optional<re::Captures> maybeCaptures;
  re::Captures captures;

  // PCRE2 for non-matching capture groups, and
  // std::string::find* for not found return a -1 size_t  
  size_t notFound{static_cast<size_t>(-1)};
  while(!stack.empty()) {
    parent = stack.top();
    toMatch = content.substr(0, parent.end);
    if((maybeCaptures = pat.match(toMatch, 0, parent.start))) {
      captures = *maybeCaptures;
      for(int i = 0; i < 4; i++) {
        log("{}, {}", captures[i].start, captures[i].end);
      }
      self = captures[0];
      matchBrace(self.end, toMatch);

      // If this is a union (seeing if the captured "u" is there)
      bool isUnion{captures[2].start != notFound};
      bool isStaticUnion{isUnion && (

        // check if "static" is in the front (seeing if the captured "s" is there)
        captures[1].start != notFound ||
        toMatch.substr(self.end, toMatch.find(';') + 1).contains("static")
      )};
      re::Capture NSCapture{captures[3]};
      bool isUnnamedNS{!isUnion &&

        // Check if the namespace name consists of only whitespace
        toMatch.substr(NSCapture.start, NSCapture.end - NSCapture.start).find_first_not_of(" \n\t") == notFound
      };
      if(isStaticUnion || isUnnamedNS) {
        // Export from parent.start to self.start
        // Move parent.start to self.end
      }

      // Unnamed union variable (union {...} var;)
      else if(isUnion && !isStaticUnion) {
        // How do I ignore this and keep parsing like nothing happened ???
      }
      
      // Named namespace
      else {
        // Export from parent.start to self.start
        // Move parent.start to self.end
      }
    }
    
    // No match (no more to export)
    else {
      // Export from self.start to self.end
      stack.pop();
    }
  }
}

void addExports(std::string& content, const Opts& opts) {
  getExports1(content);
}