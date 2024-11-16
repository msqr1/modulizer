#pragma once
#include <source_location>
#include <cstdlib>
#include "../3rdParty/fmt/include/fmt/format.h"

namespace modulizer {

extern bool verbose;
size_t rtnSize(char* _, size_t size);

template <typename... T> struct exitWithErr {
  // Overload for direct callers, source location is implied
  exitWithErr(fmt::format_string<T...> fmt, T&&... args, const std::source_location& loc = std::source_location::current()) {
    exitWithErr(loc, fmt, std::forward<T>(args)...);
  }
  // Overload for indirect callers (ie. calling from an error handling function). Custom source location is here to give correct error location
  [[noreturn]] exitWithErr(const std::source_location& loc, fmt::format_string<T...> fmt, T&&... args) {
    std::string_view filename = loc.file_name();
    filename = filename.substr(filename.find_last_of("/\\") + 1);
    fmt::print("Exception thrown at {}({}:{}): ", filename, loc.line(), loc.column());
    fmt::println(fmt, std::forward<T>(args)...);
    std::exit(1);
  }  
};
template <typename... T> exitWithErr(fmt::format_string<T...> fmt, T&&...) -> exitWithErr<T...>;
template <typename... T> exitWithErr(const std::source_location& loc, fmt::format_string<T...> fmt, T&&...) -> exitWithErr<T...>;

template <typename... T> void log(fmt::format_string<T...> fmt, T&&... args) {
  fmt::println(fmt, std::forward<T>(args)...);
}
template <typename... T> void logIfVerbose(fmt::format_string<T...> fmt, T&&... args) {
  if(verbose) log(fmt, std::forward<T>(args)...);
}

} // namespace modulizer