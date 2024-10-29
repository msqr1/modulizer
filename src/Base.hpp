#pragma once
#include <string>
#include <string_view>
#include <vector>
#include "../3rdParty/fast_io/include/fast_io.h"

namespace modulizer {

extern bool verbose;
using Str = std::string;
using StrView = std::string_view;
template <class T> using Vector = std::vector<T>;
namespace fio = fast_io;
#define log fio::io::println
#define vLog if(verbose) log
size_t rtnSize(char* _, size_t size);

} // namespace modulizer