#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "FileOp.hpp"
#include "Merger.hpp"
#include "Modularizer.hpp"
#include "Exporter.hpp"

void run(int argc, char* argv[]) {
  Opts opts{getOptsOrExit(argc, argv, verbose)};
  //readFiles(opts.inDir.c_str(), opts.hdrExtRegex, opts.srcExtRegex);
}
int main(int argc, char* argv[]) {
  try {
    run(argc, argv);
  }
  catch(const std::exception& exc) {
    log("{}", exc.what());
  }
  catch(int exitCode) {
    return exitCode;
  }
}
