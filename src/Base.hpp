#pragma once
#include <string>
#include <string_view>
#include <vector>
#include "../3rdParty/fast_io/include/fast_io.h"

namespace modulizer {
  using Str = std::string;
  using StrView = std::string_view;
  template <class T> using Vector = std::vector<T>;
  size_t rtnSize(char* _, size_t size);
} // namespace modulizer