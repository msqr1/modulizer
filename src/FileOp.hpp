#include "Base.hpp"
#include <vector>
namespace modulizer {

enum FileType {
  HeaderOrModuleIf,
  Source,
  ModuleImpl
};

struct File {
  FileType type;
  std::string_view path;
  std::string content;
};

std::vector<File> readFiles(std::string_view inDir, std::string_view hdrExtRegex, std::string_view srcExtRegex);
void writeFiles(std::string_view outDir, const std::vector<File>& files);

} // namespace modulizer