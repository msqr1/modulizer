#pragma once
#include "Regex.hpp"
#include <filesystem>
#include <string>

struct Opts {
  int duplicateIncludeMode;
  re::Pattern hdrExtRegex;
  re::Pattern srcExtRegex;
  std::filesystem::path inDir;
  std::filesystem::path outDir;
  std::string moduleInterfaceExt;
  std::string openExport;
  std::string closeExport;
  std::vector<std::string> includePaths;
};

Opts getOptsOrExit(int argc, const char* const* argv, bool& verbose);
