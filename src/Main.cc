#include "Argparse.hpp"
#include "HeaderProcessor.hpp"
#include <fstream>
namespace ap = argparse;
namespace fs = std::filesystem;
int main(int argc, char *argv[]) {
  bool merge{};
  bool verbose{};
  std::string inDir;
  std::string hdrExt;
  std::string srcExt;
  std::ios_base::sync_with_stdio(false);
  ap::ArgumentParser program("modulizer");
  program.add_description("C++20 modularizer in C++20");
  program.add_argument("inDir")
    .help("Specify input directory")
    .store_into(inDir);
  program.add_argument("-v", "--verbose")
    .help("Actiate verbose output")
    .store_into(verbose);
  program.add_argument("-m", "--merge")
    .help("Merge header and corresponding source into one header")
    .store_into(merge);
  program.add_argument("--header-extension")
    .help("Set header extension")
    .default_value(".hpp")
    .store_into(hdrExt);
  program.add_argument("--source-extension")
    .help("Set source extension")
    .default_value(".cpp")
    .store_into(srcExt);
  try {
    program.parse_args(argc, argv);
    fs::path path;
    HeaderProcessor hdrP{verbose};
    for(const auto& entry : fs::directory_iterator(inDir)) {
      path = entry.path();
      if(path.extension() == hdrExt) {
        hdrP
        .load(path)
        .sysInclude2GMF()
        ;
      }
      /*if(fs::exists(path.replace_extension(srcExt))) {
        if(merge) {
          hdrP.appendSrc(path);
        }
        else {
          
        }
      }
      hdrP
      .usrInclude2Import()
      .sysInclude2GMF()
      .write()
      ;*/
    }
  }
  catch(const std::exception& err) {
    std::cerr << err.what() << "\n" << program;
    return 1;
  }
}