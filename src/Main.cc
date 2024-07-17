#include "Argparse.hpp"
#include "HeaderProcessor.hpp"
namespace ap = argparse;
namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  bool merge{};
  bool verbose{};
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
    .help("Merge header and corresponding source into one file")
    .store_into(merge);
  program.add_argument("--header-extension")
    .help("Specify header file extension")
    .default_value(".hpp")
    .store_into(hdrExt);
  program.add_argument("--source-extension")
    .help("Specify source file extension")
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
  std::fstream file;
  HeaderProcessor hdrP;
  for(const auto& entry : fs::directory_iterator(inDir)) {
    path = entry.path();
    file.open(path);
    if(!file.is_open()) {
      std::cerr << "Unable to open " << path << "\n";
      return 1;
    }
    if(path.extension() == hdrExt) {
      hdrP << file;
      fs::resize_file(path, 0);
      hdrP
      .setModuleName(path)
      .handleInternalLinkages()
      .replaceIncludes();
      hdrP >> file;
    }
    else if(path.extension() == srcExt) {
      
    }
    file.close();
  }
}