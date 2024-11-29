#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "FileOp.hpp"
#include "Modularizer.hpp"
#include "Exporter.hpp"
#include "../3rdParty/Generator.hpp"

void run(int argc, char* argv[]) {
  Opts opts{getOptsOrExit(argc, argv, verbose)};
  for(File& file : iterateFiles(opts)) {
    // Process the files
  }
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
