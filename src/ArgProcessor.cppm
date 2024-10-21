module;
#include "../3rd-party/Argparse.hpp"
#include "../3rd-party/Toml++.hpp"
#include <string>
export module ArgProcessor;
namespace ap = argparse;

export {

struct Opts {
  bool verbose;
  bool merge;
  std::string inDir;
  std::string outDir;
  std::string hdrExtRegex;
  std::string srcExtRegex;
  std::string modExt;
  std::string openExport;
  std::string closeExport;
  Opts() : 
    verbose{false},
    merge{true},
    hdrExtRegex{"\\.h(pp|xx)?"},
    srcExtRegex{"\\.c(pp|c|xx)"},
    modExt{".cppm"},
    openExport{"export {\n"},
    closeExport{"}\n"}
  {};
};
Opts processArgs(int argc, char* argv[]) {
  ap::ArgumentParser program{"modulizer", "0.0.1"};
  std::string configPath;
  Opts opts{};
  program.add_description("Fast, automatic conversion of #include code into using modules");
  program.add_argument("inDir")
    .help("Input directory")
    .store_into(opts.inDir);
  program.add_argument("-c", "--config")
    .help("Path to a configuration file")
    .store_into(configPath);
  program.add_argument("-v", "--verbose")
    .help("Enable verbose output (debugging)")
    .store_into(opts.verbose)
    .flag();
  program.add_argument("-m", "--merge")
    .help("Merge interfaces and implementations into one entity")
    .store_into(opts.merge)
    .implicit_value(true);
  program.add_argument("-o", "--out-dir")
    .help("Output directory (defaults to input directory)")
    .store_into(opts.outDir);
  program.parse_args(argc, argv);
  
}

}