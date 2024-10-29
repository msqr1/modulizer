#include "Base.hpp"

namespace modulizer {

int main(int argc, char* argv[]) {
  vLog("hello");
  return 0;
}

} // namespace modulizer

int main(int argc, char* argv[]) {
  return modulizer::main(argc, argv);
}
