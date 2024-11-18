#include "Base.hpp"

namespace modulizer {

bool verbose;
size_t rtnSize(char *_, size_t size) {
  return size;
}
void exitOK() {
  throw EXIT_SUCCESS;
}

} // namespace modulizer

