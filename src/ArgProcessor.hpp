#include <string_view>

namespace modulizer {

struct Opts {
  bool merge;
  std::string_view inDir;
  std::string outDir;
  std::string hdrExtRegex;
  std::string srcExtRegex;
  std::string moduleInterfaceExt;
  std::string openExport;
  std::string closeExport;
};
Opts getOptsOrExit(int argc, char* argv[], bool& verbose);

} // namespace modulizer