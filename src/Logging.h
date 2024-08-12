#pragma once
#include <print>
template<typename... T> [[noreturn]] void logAndExit(std::format_string<T...> fmt, T&&... args) {
  std::println(stderr, fmt, std::forward<T>(args)...);
  std::exit(1);
}
template<typename... T> void log(std::format_string<T...> fmt, T&&... args) {
  std::println(fmt, std::forward<T>(args)...);
}