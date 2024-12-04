#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "../3rdParty/fmt/include/fmt/format.h"
#define TOML_EXCEPTIONS 0
#define TOML_ENABLE_FORMATTERS 0
#include "../3rdParty/toml++/include/toml++/toml.hpp"

namespace fs = std::filesystem;

const char* getOptVal(int optidx, int argc, char* argv[]) {
  optidx++;
  if(optidx == argc || argv[optidx][0] == '-') 
    exitWithErr("Invalid/Nonexistent value for option {}", argv[optidx - 1]);
  return argv[optidx];
}

Opts getOptsOrExit(int argc, char* argv[], bool& verbose) {
  Opts opts;
  std::string_view arg, configPath;
  configPath = "modulizer.toml";
  if(argc > 1) {
    arg = argv[1];

    // Help or version must be the first argument
    if(arg == "-h" || arg == "--help") {
      log("Modularize C++20 code");
      exitOK();
    }
    if(arg == "-v" || arg == "--version") {
      log("0.0.1");
      exitOK();
    }
    if(arg == "-c" || arg == "--config") configPath = getOptVal(1, argc, argv);
  }
  auto parseRes{toml::parse_file(configPath)};
  if(!parseRes) {
    auto err = parseRes.error();
    auto errSrc = err.source();
    exitWithErr("TOML++ error: {} at {}({}:{})", err.description(), configPath, 
    errSrc.begin.line, errSrc.begin.column);
  }
  auto config{std::move(parseRes.table())};
  opts.inDir = config["inDir"].value_or("");
  if(opts.inDir.empty()) exitWithErr("inDir must be specified");
  opts.outDir = config["outDir"].value_or("");
  if(opts.outDir.empty()) exitWithErr("outDir must be specified");
  //exitWithErr("inDir and outDir cannot be the same");

  // Default values
  verbose = config["verbose"].value_or(false);
  std::string_view hdrExtRegexStr{config["headerExtRegex"].value_or(R"(\.h(?:pp|xx)?)")};
  std::string_view srcExtRegexStr{config["sourceExtRegex"].value_or(R"(\.c(?:pp|c|xx)?)")};
  opts.hdrExtRegex.set(hdrExtRegexStr);
  opts.srcExtRegex.set(srcExtRegexStr);
  opts.moduleInterfaceExt = config["moduleInterfaceExt"].value_or(".cppm");
  opts.openExport = config["openExport"].value_or("export {\n");
  opts.closeExport = config["closeExport"].value_or("}\n");
  logIfVerbose("inDir = {}\noutDir = {}\nhdrExtRegex = {}\nsrcExtRegex = {}\nmoduleInterfaceExt = {}\nopenExport = {}\ncloseExport = {}", 
    opts.inDir.c_str(), opts.outDir.c_str(), hdrExtRegexStr, srcExtRegexStr, 
    opts.moduleInterfaceExt, opts.openExport, opts.closeExport);
  
  // Implicit move construction
  return opts;
}
