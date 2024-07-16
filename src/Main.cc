#include "Argparse.hpp"
#include <filesystem>
namespace ap = argparse;

int main(int argc, char *argv[]) {
  bool merge;
  std::string inDir;

  ap::ArgumentParser program("modulizer");
  program.add_description("C++20 modularizer, convert #include code into using C++20 modules");
  program.add_argument("inDir")
    .help("Input directory")
    .store_into(inDir);
  program.add_argument("-m", "--merge")
    .help("Merge declaration and definition into a single .ixx")
    .store_into(merge);
    
  try {
    program.parse_args(argc, argv);
  }
  catch(const std::exception& err) {
    std::cerr << err.what() << "\n" << program;
    return 1;
  }
}