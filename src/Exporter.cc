#include "Exporter.hpp"
#include "Base.hpp"
#include "ArgProcessor.hpp"
#include <string>
#include <string_view>
#include <stack>
#include "../3rdParty/Generator.hpp"
struct Export {
  size_t open;
  size_t close;
  Export(size_t open, size_t close) : open{open}, close{close} {};
  Export() {};
};

// NS represents a namespace, having an open and a close, just like an export block, so I just alias it.
using NS = Export;

// Exports for everything but static unions and anonymous namespace (sorry I can't think of a better name for this)
void getExports1(std::string_view content) {

  // Matches namespace or UNNAMED union. Capture the "s" in static to try testing if the union is static or not. Capture the "u" to see if it is a namespace or union.
  re::Pattern pat{R"((?:(?:(s)tatic|inline)\s++)?(?:(u)nion\s*|namespace(.*))\{)"};
  std::stack<NS> stack;
  stack.emplace(0, content.size());
  NS parent;
  std::string_view toMatch;
  while(!stack.empty()) {
    parent = stack.top();
    toMatch = content.substr(0, parent.close);
    std::optional<re::Captures> maybeCaptures{pat.match(toMatch, 0, parent.open)};
    stack.pop();
  }
}
void addExports(std::string& content, const Opts& opts) {
  getExports1(content);
}