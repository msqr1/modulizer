#pragma once
#define PCRE2_CODE_UNIT_WIDTH 8
#include "../3rdParty/pcre2/src/pcre2.h.generic"
#include "../3rdParty/Generator.hpp"
#include <string_view>
#include <optional>

namespace modulizer::re {

class Capture {
friend class Captures;
  Capture(const Capture&) = delete;
  Capture& operator=(const Capture&) = delete;
  Capture(size_t start, size_t end);
public:
  size_t start;
  size_t end;
};

class Captures {
friend class Pattern;
  size_t* ovector;
  Captures(const Captures&) = delete;
  Captures& operator=(const Captures&) = delete;

public:
  Captures(size_t* ovector);
  Capture operator[](int idx);
};

class Pattern {
  pcre2_code* pattern;
  pcre2_match_data* matchData;

  void free();
public:
  Pattern(const Pattern&) = delete;
  Pattern& operator=(const Pattern&) = delete;
  Pattern(Pattern&& other);
  Pattern(std::string_view pat, uint32_t opts = 0);
  Pattern();
  ~Pattern();

  void set(std::string_view pat, uint32_t opts = 0);

  // Base implementation
  std::optional<Captures> match(std::string_view subject, size_t startoffset, uint32_t opts = 0);
  // Match with startoffset = 0
  std::optional<Captures> match(std::string_view subject, uint32_t opts = 0);

  // Get all matches in the string
  cppcoro::generator<Captures> matchAll(std::string_view subject, uint32_t opts = 0);
};

} // namespace modulizer::re

