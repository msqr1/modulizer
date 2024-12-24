#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "../3rdParty/fmt/include/fmt/format.h"
#define TOML_EXCEPTIONS 0
#define TOML_ENABLE_FORMATTERS 0
#include "../3rdParty/toml++/include/toml++/toml.hpp"
#include <string_view>

namespace fs = std::filesystem;
namespace {

std::string_view getOptVal(int optidx, int argc, const char* const* argv) {
  optidx++;
  if(optidx == argc || argv[optidx][0] == '-') {
    exitWithErr("Invalid/Nonexistent value for option {}", argv[optidx - 1]);
  }
  return argv[optidx];
}

} // namespace

Opts getOptsOrExit(int argc, const char* const* argv, bool& verbose) {
  Opts opts;
  std::string_view configPath{"modulizer.toml"};
  {
    std::string_view arg;
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
  }
  const toml::parse_result parseRes{toml::parse_file(configPath)};
  if(!parseRes) {
    const toml::parse_error err{parseRes.error()};
    const toml::source_region errSrc{err.source()};
    exitWithErr("TOML++ error: {} at {}({}:{})", err.description(), configPath, 
    errSrc.begin.line, errSrc.begin.column);
  }
  const toml::table config{std::move(parseRes.table())};
  auto getTypeCk = [&]<typename keyType>(std::string_view key) {
    const toml::node_view<const toml::node> node{config[key]};

    // TOML need std::string, not const char*, string_view, etc.
    if constexpr(std::is_convertible_v<keyType, std::string>) {
      if(node.is_string()) return node.ref<std::string>();
    }
    else if(node.is<keyType>()) return node.ref<keyType>();
    exitWithErr("Incorrect TOML type for {}", key);
  }; 
  auto getOrDefault = [&]<typename T>(std::string_view key, T&& defaultVal) {
    if(config.contains(key)) return getTypeCk.template operator()<T>(key);
    if constexpr(std::is_convertible_v<T, std::string>) return std::string{defaultVal};
    else return std::forward<T>(defaultVal);
  };
  auto getStr = [&](std::string_view key) {
    if(!config.contains(key)) exitWithErr("{} must be specified", key);
    return getTypeCk.template operator()<std::string>(key);
  };
  opts.inDir = getStr("inDir");
  opts.outDir = getStr("outDir");
  verbose = getOrDefault("verbose", false);
  opts.duplicateIncludeMode = getOrDefault("duplicateIncludeMode", 0L);
  opts.hdrExtRegex.set(getOrDefault("hdrExtRegex", R"(\.h(?:pp|xx)?)"));
  opts.srcExtRegex.set(getOrDefault("srcExtRegex", R"(\.c(?:pp|c|xx)?)"));
  opts.moduleInterfaceExt = getOrDefault("moduleInterfaceExt", ".cppm");
  opts.openExport = getOrDefault("openExport", "export{\n");
  opts.closeExport = getOrDefault("closeExport", "}\n");
  if(config.contains("includePaths")) {
    const toml::array includePathsArr{*config.get_as<toml::array>("includePaths")};
    for(const toml::node& elem : includePathsArr) {
      if(!elem.is_string()) exitWithErr("includePaths must only contain strings");
      opts.includePaths.emplace_back(elem.ref<std::string>());
    }
  }
  return opts;
}
