#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "../3rdParty/fmt/include/fmt/format.h"
#define TOML_EXCEPTIONS 0
#define TOML_ENABLE_FORMATTERS 0
#include "../3rdParty/toml++/include/toml++/toml.hpp"

namespace modulizer {

const char* getOptVal(int& optidx, int argc, char* argv[]) {
  optidx++;
  if(optidx == argc || argv[optidx][0] == '-') 
    exitWithErr("Invalid value for option {}", argv[optidx - 1]);
  return argv[optidx];
}
Opts getOptsOrExit(int argc, char* argv[], bool& verbose) {
  if(argc < 2) exitWithErr("No argument specified");
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
  opts.inDir = arg;
  configPath = "modulizer.toml";
  for(int optidx{2}; optidx < argc; ++optidx) {
    arg = argv[optidx];
    if(arg == "-c" || arg == "--config") configPath = getOptVal(optidx, argc, argv);
    else exitWithErr("Invalid option {}", arg);
  }
  auto parseRes{toml::parse_file(configPath)};
  if(!parseRes) {
    auto err = parseRes.error();
    auto errSrc = err.source();
  exitWithErr("TOML++ error: {} @ {}({}:{})", err.description(), configPath, errSrc.begin.line, errSrc.begin.column);
  }
  auto config{std::move(parseRes.table())};
  // Default values
  opts.outDir = config["outDir"].value_or("");
  if(opts.outDir.empty()) exitWithErr("outDir must be specified/valid");
  verbose = config["verbose"].value_or(false);
  opts.merge = config["merge"].value_or(false);
  opts.hdrExtRegex = config["headerExtRegex"].value_or(R"(\.h(pp|xx)?)");
  opts.srcExtRegex = config["sourceExtRegex"].value_or(R"(\.c(pp|c|xx)");
  opts.moduleInterfaceExt = config["moduleInterfaceExt"].value_or(".cppm");
  opts.openExport = config["openExport"].value_or(".export {\n");
  opts.closeExport = config["closeExport"].value_or("}\n");
  return opts;
}

} // namespace modulizer