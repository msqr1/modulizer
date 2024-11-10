#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "../3rdParty/fmt/include/fmt/format.h"
#include <string>
using namespace modulizer;

int run(int argc, char* argv[]) {
  Opts opts{getOptsOrExit(argc, argv, verbose)};
  log(fmt::format("merge = {}\ninDir = {}\noutDir = {}\nhdrExtRegex = {}\nsrcExtRegex = {}\nmoduleInterfaceExt = {}\nopenExport = {}\ncloseExport = {}", opts.merge, opts.inDir, opts.outDir, opts.hdrExtRegex, opts.srcExtRegex, opts.moduleInterfaceExt, opts.openExport, opts.closeExport));
  return 0;
}
int main(int argc, char* argv[]) {
  try {
    return run(argc, argv);
  }
  catch(const std::string& errMsg) { // For modulizer exceptions
    log(errMsg);
  }
  catch(const std::exception& exc) { // For other exceptions
    log(std::string_view(exc.what()));
  }
  return 1;
}
