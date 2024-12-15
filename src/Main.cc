#include "Base.hpp"
#include "ArgProcessor.hpp"
#include "FileOp.hpp"
#include "Modularizer.hpp"
#include "Exporter.hpp"
#include "../3rdParty/Generator.hpp"

void run(int argc, const char* const* argv) {
  const Opts opts{getOptsOrExit(argc, argv, verbose)};
  for(File& file : iterateFiles(opts)) {
    addExports(file.content, opts);
  }
}

int main(int argc, const char* const* argv) {
  try {
    run(argc, argv);
  }
  catch(const std::exception& exc) {
    log("std::exception thrown: {}", exc.what());
  }
  catch(int exitCode) {
    return exitCode;
  }
}
