#include "Base.hpp"
#include "../3rdParty/fmt/include/fmt/format.h"
namespace modulizer {

bool verbose;
size_t rtnSize(char *_, size_t size) {
  return size;
}
void throwErr(const std::string& msg, const std::source_location& loc) {
  throw fmt::format("Exception thrown @ {}({}:{}): {}", loc.file_name(), loc.line(), loc.column(), msg);
}

}

