#include "Exporter.hpp"
#include "Base.hpp"
#include "ArgProcessor.hpp"
#include <string>
#include <string_view>
#include <vector>

struct Export {
  size_t open;
  size_t close;
};
using Exports = std::vector<Export>;

Exports getExports(std::string_view content) {
  Exports exps;
  return exps;
}
void addExports(std::string& content, const Opts& opts) {
  Exports exps{getExports(content)};
  for(Export exp : exps) {
    
  }
}