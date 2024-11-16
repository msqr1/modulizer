#pragma once
#include <source_location>
#include <cstdlib>
#include "../3rdParty/fmt/include/fmt/format.h"

namespace modulizer {

extern bool verbose;
size_t rtnSize(char* _, size_t size);

template <typename... T> struct exitWithErr {
  [[noreturn]] exitWithErr(fmt::format_string<T...> fmt, T&&... args, const std::source_location& loc = std::source_location::current()) {
    fmt::print("Exception thrown @ {}({}:{}): ", loc.file_name(), loc.line(), loc.column());
    fmt::println(fmt, std::forward<T>(args)...);
    std::exit(1);
  }
};
template <typename... T> exitWithErr(fmt::format_string<T...> fmt, T&&...) -> exitWithErr<T...>;
template <typename... T> void log(fmt::format_string<T...> fmt, T&&... args) {
  fmt::println(fmt, std::forward<T>(args)...);
}
template <typename... T> void logIfVerbose(fmt::format_string<T...> fmt, T&&... args) {
  if(verbose) log(fmt, std::forward<T>(args)...);
}

} // namespace modulizer