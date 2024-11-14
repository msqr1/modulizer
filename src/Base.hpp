#pragma once
#include <source_location>
#include <string>
#include "../3rdParty/fmt/include/fmt/format.h"

namespace modulizer {

extern bool verbose;
size_t rtnSize(char* _, size_t size);
[[noreturn]] void exitWithErr(const std::string& msg, const std::source_location& loc = std::source_location::current());
template <typename... T> void log(fmt::format_string<T...> fmt, T&&... args) {
  fmt::println(fmt, std::forward<T>(args)...);
}
template <typename... T> void logIfVerbose(fmt::format_string<T...> fmt, T&&... args) {
  if(verbose) log(fmt, args...);
}

} // namespace modulizer