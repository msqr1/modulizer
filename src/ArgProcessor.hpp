#include <string_view>
namespace modulizer {

struct Opts {
  bool merge;
  std::string_view inDir;
  std::string_view outDir;
  std::string_view hdrExtRegex;
  std::string_view srcExtRegex;
  std::string_view moduleInterfaceExt;
  std::string_view openExport;
  std::string_view closeExport;
};
Opts getOptsOrExit(int argc, char* argv[], bool& verbose);

}