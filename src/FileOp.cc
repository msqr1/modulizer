#include "FileOp.hpp"
#include <filesystem>

namespace fs = std::filesystem;
namespace modulizer {

std::vector<File> readFiles(std::string_view inDir, std::string_view hdrExtRegex, std::string_view srcExtRegex) {
  std::vector<File> files;
  File file;
  fs::path p;
  for(const auto& ent : fs::recursive_directory_iterator(inDir)) {
    p = ent.path();
    file.path = p.c_str();
    file.path = file.path.substr(inDir.length());
    
  }
  return files;
}

} // namespace modulizer