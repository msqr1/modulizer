#pragma once
#include "Base.hpp"
#include <vector>
#include <filesystem>

namespace re {
  class Pattern;
}
namespace cppcoro {
  template<typename T> class generator;
}
struct Opts;

enum FileType {
  Header,
  Source,
};

struct File {
  FileType type;
  std::filesystem::path path;
  std::string content;
};

cppcoro::generator<File&> iterateFiles(const Opts& opts);
