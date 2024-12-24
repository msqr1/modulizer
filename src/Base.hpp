#pragma once
#include "../3rdParty/fmt/include/fmt/format.h"
#include <source_location>
#include <filesystem>

extern bool verbose;

size_t rtnSize(char* _, size_t size);

// PCRE2 for non-matching capture groups, and
// std::string(_view)::find* for not found return a -1 size_t (std::string::npos)
constexpr size_t notFound{static_cast<size_t>(-1)};

[[noreturn]] void exitOK();

template <typename... T> struct exitWithErr {

  // Overload for direct callers, source location is implied
  [[noreturn]] exitWithErr(fmt::format_string<T...> fmt, T&&... args, 
  const std::source_location& loc = std::source_location::current()) {
    exitWithErr(loc, fmt, std::forward<T>(args)...);
  }
  
  // Overload for indirect callers (ie. calling from an error handling function). 
  // Custom source location is here to give correct error location
  [[noreturn]] exitWithErr(const std::source_location& loc, fmt::format_string<T...> fmt
    , T&&... args) {
    std::filesystem::path p{loc.file_name()};
    fmt::print("Exception thrown at {}({}:{}): ", p.filename().native(), loc.line(), loc.column());
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