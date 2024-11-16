#define PCRE2_CODE_UNIT_WIDTH 8
#include "../3rdParty/pcre2/src/pcre2.h.generic"
#include <string_view>

namespace modulizer::re {

struct Pattern {
  pcre2_code* pat;
  Pattern(std::string_view pat, uint32_t opts = 0);
  ~Pattern();
};

// Never pass Patterns by anything but const reference

void match(const Pattern& pattern, std::string_view subject, uint32_t opts = 0);

}

