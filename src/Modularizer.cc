#include "Modularizer.hpp"
#include "Base.hpp"
#include "Regex.hpp"
#include "ArgProcessor.hpp"
#include "FileOp.hpp"
#include <variant>
#include <string>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

// Convert a path to a module name relative to root
std::string path2ModuleName(const fs::path& p) {
 
}

void processIncludes(std::string_view content) {

  // Pattern to find #includes
  const re::Pattern pat{R"(#\s*include\s*[<"]([^>"]+)[>"])"};

  std::optional<re::Captures> maybeCaptures;
  while((maybeCaptures = pat.match(content))) {

  }
}

size_t insertGMF(std::string& content, const fs::path& p) {

}

}

void modularize(File& file, const Opts& opts) {
  logIfVerbose("Inserting GMF...");
}
