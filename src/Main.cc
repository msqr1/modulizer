#include "Argparse.hpp"
#include "HeaderProcessor.hpp"
#include <fstream>
namespace ap = argparse;
namespace fs = std::filesystem;
int main(int argc, char *argv[]) {
  bool merge{};
  bool verbose{};
  int indent{};
  std::string inDir;
  std::string hdrExt;
  std::string srcExt;

  ap::ArgumentParser program("modulizer");
  program.add_description("C++20 modularizer tool, convert #include code into using C++20 modules");
  program.add_argument("inDir")
    .help("Specify input directory")
    .store_into(inDir);
  program.add_argument("-v", "--verbose")
    .help("Actiate verbose output")
    .store_into(verbose);
  program.add_argument("-m", "--merge")
    .help("Merge header and corresponding source into one header")
    .store_into(merge);
  program.add_argument("-i", "--indent")
    .help("Set indentation size for export blocks (and definitions later)")
    .scan<'i', int>()
    .store_into(indent);
  program.add_argument("--header-extension")
    .help("Set header header extension")
    .default_value(".hpp")
    .store_into(hdrExt);
  program.add_argument("--source-extension")
    .help("Set source header extension")
    .default_value(".cpp")
    .store_into(srcExt);
  try {
    program.parse_args(argc, argv);
  }
  catch(const std::exception& err) {
    std::cerr << err.what() << "\n" << program;
    return 1;
  }
  fs::path path;
  fs::path srcPath;
  std::fstream header;
  HeaderProcessor hdrP{verbose, indent};
  for(const auto& entry : fs::directory_iterator(inDir)) {
    path = entry.path();
    header.open(path);
    if(!header.is_open()) {
      std::cerr << "Unable to open " << path << "\n";
      return 1;
    }
    if(path.extension() == hdrExt) {
      hdrP << header;
      //fs::resize_header(path, 0);
      hdrP
      .include2Import();
      hdrP >> header;
    }
    header.close();
  }
}