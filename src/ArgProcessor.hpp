#include <string_view>
#include "Regex.hpp"

namespace modulizer {

struct Opts {
  bool merge;
  re::Pattern hdrExtRegex;
  re::Pattern srcExtRegex;
  std::string_view inDir;
  std::string outDir;
  std::string moduleInterfaceExt;
  std::string openExport;
  std::string closeExport;
};

Opts getOptsOrExit(int argc, char* argv[], bool& verbose);

} // namespace modulizer