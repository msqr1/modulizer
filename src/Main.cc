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
  catch(const Str& err) {
    log(err);
  }
  catch(const std::exception& exc) {
    log(StrView(exc.what()));
  }
}
