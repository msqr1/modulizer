#pragma once
#include "Regex.hpp"
#include <filesystem>
#include <string_view>

namespace modulizer {

struct Opts {
  bool merge;
  re::Pattern hdrExtRegex;
  re::Pattern srcExtRegex;
  std::filesystem::path inDir;
  std::filesystem::path outDir;
  std::string moduleInterfaceExt;
  std::string openExport;
  std::string closeExport;
};

Opts getOptsOrExit(int argc, char* argv[], bool& verbose);

} // namespace modulizer
