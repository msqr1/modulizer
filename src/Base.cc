#include "Base.hpp"
#include <format>
namespace modulizer {

bool verbose;
size_t rtnSize(char *_, size_t size) {
  return size;
}
void throwErr(const Str& msg, const std::source_location& loc) {
  throw std::format("Exception thrown @ {}({}:{}): {}", loc.file_name(), loc.line(), loc.column(), msg);
}

}

