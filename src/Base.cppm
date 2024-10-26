module;
#include <string>
#include <string_view>
#include <vector>
export module Base;
export {
  using Str = std::string;
  using StrView = std::string_view;
  template <typename T> using Vector = std::vector<T>;
}
