#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "../3rdParty/fmt/include/fmt/format.h"
#define TOML_EXCEPTIONS 0
#define TOML_ENABLE_FORMATTERS 0
#include "../3rdParty/Toml++.hpp"


namespace modulizer {

const char* getOptVal(int& optidx, int argc, char* argv[]) {
  optidx++;
  if(optidx == argc || argv[optidx][0] == '-') 
    throwErr(fmt::format("Invalid value for {}", argv[optidx - 1]));
  else return argv[optidx];
}
Opts getOptsOrExit(int argc, char* argv[], bool& verbose) {
  if(argc < 2) throwErr("No argument specified");
  Opts opts;

  std::string_view arg, configPath;
  arg = argv[1];
  // help or version must be the first argument, else it is inDir
  if(arg == "-h" || arg == "--help") {
    log("Modularize C++20 code");
    std::exit(0);
  }
  if(arg == "-v" || arg == "--version") {
    log("0.0.1");
    std::exit(0);
  }
  opts.inDir = opts.outDir = arg;
  for(int optidx{2}; optidx < argc; ++optidx) {
    arg = argv[optidx];
    if(arg == "-c" || arg == "--config") configPath = getOptVal(optidx, argc, argv);
    else throwErr(fmt::format("Invalid option {}", arg));
  }
  if(configPath.empty()) return opts;
  auto parseRes{toml::parse_file(configPath)};
  if(!parseRes) {
    auto err{parseRes.error()};
    throwErr(fmt::format("TOML++ error {0}", err.description()));
  }
  auto config{std::move(parseRes.table())};

  // Default values
  verbose = config["verbose"].value_or(false);
  opts.merge = config["merge"].value_or(false);
  opts.outDir = config["outDir"].value_or(opts.outDir);
  opts.hdrExtRegex = config["headerExtRegex"].value_or(R"(\.h(pp|xx)?)");
  opts.srcExtRegex = config["sourceExtRegex"].value_or(R"(\.c(pp|c|xx)");
  opts.moduleInterfaceExt = config["moduleInterfaceExt"].value_or(".cppm");
  opts.openExport = config["openExport"].value_or(".export {\n");
  opts.closeExport = config["closeExport"].value_or("}\n");
  return opts;
}

}