#include "FileOp.hpp"

namespace fs = std::filesystem;
namespace modulizer {

std::vector<File> readFiles(std::string_view inDir, re::Pattern& hdrExtRegex, re::Pattern& srcExtRegex) {
  std::vector<File> files;
  File file;
  std::string_view ext;
  std::string_view pathStr;
  size_t prefixLen = inDir.length() + 1;
  for(const auto& ent : fs::recursive_directory_iterator(inDir)) {
    pathStr = ent.path().c_str();
    pathStr.remove_prefix(prefixLen);
    file.path = pathStr;
    ext = file.path.extension().c_str();
    if(hdrExtRegex.match(ext)) {
      log("Headers: {}", pathStr);
    }
    else if(srcExtRegex.match(ext)) {
      log("Source: {}", pathStr);
    }
  }
  return files;
}

} // namespace modulizer