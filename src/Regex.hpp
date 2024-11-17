#define PCRE2_CODE_UNIT_WIDTH 8
#include "../3rdParty/pcre2/src/pcre2.h.generic"
#include "../3rdParty/Generator.hpp"
#include <string_view>
#include <optional>

namespace modulizer::re {

struct Capture {
  size_t start;
  size_t end;

  Capture(size_t start, size_t end);
};

struct Captures {
  size_t* ovector;
  Captures(size_t* ovector);
  Capture operator[](int idx);
};

struct Pattern {
  pcre2_code* pattern;
  pcre2_match_data* matchData;

  Pattern(std::string_view pat, uint32_t opts = 0);
  ~Pattern();

  // Base implementation
  std::optional<Captures> match(std::string_view subject, size_t startoffset, uint32_t opts = 0);

  // atch with startoffset = 0
  std::optional<Captures> match(std::string_view subject, uint32_t opts = 0);

  // Get all matches in the string
  std::generator<Captures> matchAll(std::string_view subject, uint32_t opts = 0);
};

} // namespace modulizer::re

