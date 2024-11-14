#include "Base.hpp"
#include "../3rdParty/fmt/include/fmt/format.h"
#include <cstdlib>
namespace modulizer {

bool verbose;
size_t rtnSize(char *_, size_t size) {
  return size;
}
void exitWithErr(const std::string& msg, const std::source_location& loc) {
  fmt::println("Exception thrown @ {}({}:{}): {}", loc.file_name(), loc.line(), loc.column(), msg);
  std::exit(1);
}

}

