#pragma once
#include "../3rdParty/fmt/include/fmt/format.h"
#include <source_location>
#include <filesystem>

extern bool verbose;
constexpr char pathSeparator{std::filesystem::path::preferred_separator};

size_t rtnSize(char* _, size_t size);

[[noreturn]] void exitOK();

template <typename... T> struct exitWithErr {

  // Overload for direct callers, source location is implied
  exitWithErr(fmt::format_string<T...> fmt, T&&... args, const std::source_location& loc = std::source_location::current()) {
    exitWithErr(loc, fmt, std::forward<T>(args)...);
  }
  
  // Overload for indirect callers (ie. calling from an error handling function). 
  // Custom source location is here to give correct error location
  [[noreturn]] exitWithErr(const std::source_location& loc, fmt::format_string<T...> fmt
    , T&&... args) {
    std::string_view filename{loc.file_name()};
    filename.remove_prefix(filename.find_last_of(pathSeparator));
    fmt::print("Exception thrown at {}({}:{}): ", filename, loc.line(), loc.column());
    fmt::println(fmt, std::forward<T>(args)...);
    throw 1;
  }  
};
template <typename... T> exitWithErr(fmt::format_string<T...> fmt, T&&...)
  -> exitWithErr<T...>;
template <typename... T> exitWithErr(const std::source_location& loc,
  fmt::format_string<T...> fmt, T&&...) -> exitWithErr<T...>;

template <typename... T> void log(fmt::format_string<T...> fmt, T&&... args) {
  fmt::println(fmt, std::forward<T>(args)...);
}

template <typename... T> void logIfVerbose(fmt::format_string<T...> fmt, T&&... args) {
  if(verbose) log(fmt, std::forward<T>(args)...);
}