#include "Base.hpp"
#include "ArgProcessor.hpp"
using namespace modulizer;

int run(int argc, char* argv[]) {
  getOptsOrExit(argc, argv, verbose);
  return 0;
}
int main(int argc, char* argv[]) {
  try {
    return run(argc, argv);
  }
  
  catch(const Str& errMsg) { // For modulizer exceptions
    log(errMsg);
  }
  catch(const std::exception& exc) { // For other exceptions
    log(StrView(exc.what()));
  }
  return 1;
}
