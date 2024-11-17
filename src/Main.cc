#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "FileOp.hpp"
#include "Merger.hpp"
#include "Modularizer.hpp"
#include "Exporter.hpp"

using namespace modulizer;

void run(int argc, char* argv[]) {
  /*Opts opts{getOptsOrExit(argc, argv, verbose)};
  logIfVerbose("merge = {}\ninDir = {}\noutDir = {}\nhdrExtRegex = {}\nsrcExtRegex = {}\nmoduleInterfaceExt = {}\nopenExport = {}\ncloseExport = {}", opts.merge, opts.inDir, opts.outDir, opts.hdrExtRegex, opts.srcExtRegex, opts.moduleInterfaceExt, opts.openExport, opts.closeExport);
  readFiles(opts.inDir, opts.hdrExtRegex, opts.srcExtRegex);*/
}
int main(int argc, char* argv[]) {
  try {
    run(argc, argv);
    return 0;
  }
  catch(const std::exception& exc) {
    log("{}", exc.what());
  }
  return 1;
}
