#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "../3rdParty/fmt/include/fmt/format.h"
#define TOML_EXCEPTIONS 0
#define TOML_ENABLE_FORMATTERS 0
#include "../3rdParty/toml++/include/toml++/toml.hpp"

namespace fs = std::filesystem;
namespace {

std::string_view getOptVal(int optidx, int argc, const char* const* argv) {
  optidx++;
  if(optidx == argc || argv[optidx][0] == '-') {
    exitWithErr("Invalid/Nonexistent value for option {}", argv[optidx - 1]);
  }
  return argv[optidx];
}

// For lambda overloading
template<class... T> struct overload : T... { using T::operator()...; };
template<class... T> overload(T...) -> overload<T...>;

} // namespace

Opts getOptsOrExit(int argc, const char* const* argv, bool& verbose) {
  Opts opts;
  std::string_view arg;
  std::string_view configPath{"modulizer.toml"};
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
  const toml::parse_result parseRes{toml::parse_file(configPath)};
  if(!parseRes) {
    const toml::parse_error err{parseRes.error()};
    const toml::source_region errSrc{err.source()};
    exitWithErr("TOML++ error: {} at {}({}:{})", err.description(), configPath, 
    errSrc.begin.line, errSrc.begin.column);
  }
  const toml::table config{std::move(parseRes.table())};
  toml::node_view<const toml::node> node;
  auto ckAndGetMustHave = [&node, &config](std::string_view key){
    if(config.contains(key)) {
      if(node = config[key], node.is_string()) return node.ref<std::string>();
      else exitWithErr("Incorrect TOML type for {}", key);
    }
    exitWithErr("{} must be specified", key);
  };
  opts.inDir = ckAndGetMustHave("inDir");
  opts.outDir = ckAndGetMustHave("outDir");
  auto ckAndGet = overload (
    [&node, &config]<typename T>(std::string_view key, T&& defaultVal) {
      if(config.contains(key)) {
        if(node = config[key], node.is<T>()) return node.ref<T>();
        else exitWithErr("Incorrect TOML type for {}", key);
      }
      return defaultVal;
    },
    
    // Strings need special care because node.is() doesn't recognize const char*
    // defaultVal is const char* because it should always be a string literal in 
    // the code
    [&node, &config](std::string_view key, const char* defaultVal) -> std::string {
      if(config.contains(key)) {
        if(node = config[key], node.is_string()) return node.ref<std::string>();
        else exitWithErr("Incorrect TOML type for {}", key);
      }
      return defaultVal;
    }
  );
  verbose = ckAndGet("verbose", false);
  opts.duplicateIncludeMode = ckAndGet("duplicateIncludeMode", static_cast<int64_t>(0));
  opts.hdrExtRegex.set(ckAndGet("hdrExtRegex", R"(\.h(?:pp|xx)?)"));
  opts.srcExtRegex.set(ckAndGet("srcExtRegelax", R"(\.c(?:pp|c|xx)?)"));
  opts.moduleInterfaceExt = ckAndGet("moduleInterfaceExt", ".cppm");
  opts.openExport = ckAndGet("openExport", "export{\n");
  opts.closeExport = ckAndGet("closeExport", "}\n");

  const toml::array includePathsArr{*config.get_as<toml::array>("includePaths")};
  for(const toml::node& elem : includePathsArr) {
    if(!elem.is_string()) exitWithErr("includePaths must only contain strings");
    opts.includePaths.emplace_back(elem.ref<std::string>());
  }
  return opts;
}
