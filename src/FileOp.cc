#include "FileOp.hpp"

namespace fs = std::filesystem;
namespace modulizer {

std::vector<File> readFiles(std::string_view inDir, re::Pattern& hdrExtRegex, re::Pattern& srcExtRegex) {
  std::vector<File> files;
  File file;
  std::string_view ext;
  for(const auto& ent : fs::recursive_directory_iterator(inDir)) {
    file.path = ent.path();
    ext = file.path.extension().c_str();
    if(hdrExtRegex.match(ext)) {

    }
    else if(srcExtRegex.match(ext)) {
      
    }
  }
  return files;
}

} // namespace modulizer