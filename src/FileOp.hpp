#pragma once
#include "Base.hpp"
#include <vector>

namespace re {
  class Pattern;
}

enum FileType {
  HeaderOrModuleIf,
  Source,
  ModuleImpl
};

struct File {
  FileType type;
  std::filesystem::path relPath;
  std::string content;
};

std::vector<File> readFiles(std::string_view inDir, re::Pattern& hdrExtRegex, re::Pattern& srcExtRegex);
void writeFiles(std::string_view outDir, const std::vector<File>& files);