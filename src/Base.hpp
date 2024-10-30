#pragma once
#include <vector>
#include <source_location>
#include "../3rdParty/fast_io/include/fast_io.h"

namespace modulizer {

extern bool verbose;
using Str = std::string;
using StrView = std::string_view;
template <class T> using Vector = std::vector<T>;

namespace fio = fast_io;

size_t rtnSize(char* _, size_t size);
[[noreturn]] void throwErr(const Str& msg, const std::source_location& loc = std::source_location::current());
template <class T, class... Args> void log(T&& t, Args&&... args) {
  fio::io::println(t, args...);
}
template <class T, class... Args> void logIfVerbose(T&& t, Args&&... args) {
  if(verbose) log(t, args...);
}

} // namespace modulizer