#include "FileOp.hpp"
#include "Regex.hpp"
#include "ArgProcessor.hpp"
#include "../3rdParty/Generator.hpp"
#include <fstream>

namespace fs = std::filesystem;

cppcoro::generator<File&> iterateFiles(const Opts& opts) {
  const size_t prefixLen = opts.inDir.native().length() + 1;
  File file;
  fs::path path;
  std::string_view pathView;
  std::ifstream ifs;
  std::ofstream ofs;
  logIfVerbose("Scanning input directory...");
  for(const auto& ent : fs::recursive_directory_iterator(opts.inDir)) {
    path = ent.path();
    logIfVerbose("Current file: {}", path.native());
    pathView = path.extension().native();
    if(opts.hdrExtRegex.match(pathView)) file.type = FileType::Header;
    else if(opts.srcExtRegex.match(pathView)) file.type = FileType::Source;
    else {
      logIfVerbose("Is ignored, not header/source");
      continue;
    }

    logIfVerbose("Is a header/source, trying to read...");
    ifs.open(path);
    if(!ifs) exitWithErr("Unable to open {} for reading", path.native());
    size_t fileSize{fs::file_size(path)};
    file.content.resize_and_overwrite(fileSize, rtnSize);
    ifs.read(file.content.data(), fileSize);
    if(ifs.fail() || ifs.bad()) exitWithErr("Unable to read from {}", path.native());
    ifs.close();
    co_yield file;

    logIfVerbose("Trying to write...");
    if(file.type == FileType::Header) path.replace_extension(opts.moduleInterfaceExt);
    pathView = path.native();
    pathView.remove_prefix(prefixLen);
    path = opts.outDir / pathView;
    fs::create_directories(path.parent_path());
    ofs.open(path);
    if(!ofs) exitWithErr("Unable to open {} for writing", path.native());
    ofs.write(file.content.data(), file.content.length());
    if(ofs.fail() || ofs.bad()) exitWithErr("Unable to write to {}", path.native());
    ofs.close();
  }
}