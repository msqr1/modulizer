#pragma once
#include "Regex.hpp"
#include <filesystem>
#include <string>

struct Opts {
  re::Pattern hdrExtRegex;
  re::Pattern srcExtRegex;
  std::filesystem::path inDir;
  std::filesystem::path outDir;
  std::string moduleInterfaceExt;
  std::string openExport;
  std::string closeExport;
};

Opts getOptsOrExit(int argc, const char* const* argv, bool& verbose);
