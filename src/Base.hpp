#pragma once
#include <source_location>
#include <string>
#include "../3rdParty/fast_io/include/fast_io.h"

namespace modulizer {

extern bool verbose;
size_t rtnSize(char* _, size_t size);
[[noreturn]] void throwErr(const std::string& msg, const std::source_location& loc = std::source_location::current());
template <class T, class... Args> void log(T&& t, Args&&... args) {
  fast_io::io::println(t, args...);
}
template <class T, class... Args> void logIfVerbose(T&& t, Args&&... args) {
  if(verbose) log(t, args...);
}

} // namespace modulizer