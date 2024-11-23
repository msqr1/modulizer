#include "FileOp.hpp"
#include "Regex.hpp"
#include <fstream>

namespace fs = std::filesystem;
namespace modulizer {

std::vector<File> readFiles(std::string_view inDir, re::Pattern& hdrExtRegex, re::Pattern& srcExtRegex) {
  std::vector<File> files;
  size_t prefixLen = inDir.length() + 1;
  File file;
  std::string_view pathView;
  std::ifstream in;
  for(const auto& ent : fs::recursive_directory_iterator(inDir)) {
    pathView = ent.path().c_str();
    pathView.remove_prefix(prefixLen);
    file.relPath = pathView;
    pathView = file.relPath.extension().c_str();  // pathView is now the file extension
    if(hdrExtRegex.match(pathView)) file.type = FileType::HeaderOrModuleIf;
    else if(srcExtRegex.match(pathView)) file.type = FileType::Source;
    else continue;

    size_t fileSize{fs::file_size(ent.path())};
    file.content.resize_and_overwrite(fileSize, rtnSize);
    in.open(ent.path());
    in.read(file.content.data(), fileSize);
    in.close();
  }
  return files;
}

} // namespace modulizer