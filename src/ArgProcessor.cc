#include "Base.hpp"
#include "ArgProcessor.hpp"
#include <exception>
#include <format>

namespace modulizer {

const char* getVal(int& optidx, int argc, char* argv[]) {
  optidx++;
  if(optidx == argc || argv[optidx][0] == '-') 
    throwErr(std::format("Invalid value for {}", argv[optidx - 1]));
  else return argv[optidx];
}
Opts::Opts() : 
  merge{false},
  hdrExtRegex{R"(\\.h(pp|xx)?)"},
  srcExtRegex{R"(\.c(pp|c|xx))"},
  moduleInterfaceExt{".cppm"},
  openExport{"export {\n"},
  closeExport{"}\n"}
{}
Opts getOptsOrExit(int argc, char* argv[], bool& verbose) {
  if(argc < 2) throwErr("No argument specified");
  Opts opts;
  StrView arg, configPath;
  arg = argv[1];
  // help or version must be the first argument, else it is inDir
  if(arg == "-h" || arg == "--help") {
    log("Help message here");
    std::exit(0);
  }
  else if(arg == "-V" || arg == "--version") {
    log("0.0.1");
    std::exit(0);
  }
  else opts.inDir = arg;
  for(int optidx{2}; optidx < argc; ++optidx) {
    arg = argv[optidx];
    if(arg == "-v" || arg == "--verbose") verbose = true;
    else if(arg == "-m" || arg == "--merge") opts.merge = true;
    else if(arg == "-c" || arg == "--config") {
      configPath = getVal(optidx, argc, argv);
      continue;
    }
    else if(arg == "-o" || arg == "--out-dir") {
      opts.outDir = getVal(optidx, argc, argv);
      continue;
    }
    else if(arg == "--header-ext-regex") {
      opts.hdrExtRegex = getVal(optidx, argc, argv);
      continue;
    }
    else if(arg == "--source-ext-regex") {
      opts.srcExtRegex = getVal(optidx, argc, argv);
      continue;
    }
    else if(arg == "--module-interface-ext") {
      opts.moduleInterfaceExt = getVal(optidx, argc, argv);
      continue;
    }
    else {
      throwErr(std::format("Unknown option {}", arg));
    }
  }
  return opts;
}

}