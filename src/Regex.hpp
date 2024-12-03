#pragma once
#include <string_view>
#include <optional>

class pcre2_real_code_8;
class pcre2_real_match_data_8;
namespace cppcoro {
  template<typename T> class generator;
}

namespace re {

class Capture {
friend class Captures;
  Capture(const Capture&) = delete;
  Capture& operator=(const Capture&) = delete;
  Capture(size_t start, size_t end);
public:
  size_t start;
  size_t end;
};

// Captures doesn't own anything, it's just a view over the ovector
class Captures {
friend class Pattern;
  int pairCnt;
  size_t* ovector;
public:
  Captures();
  Captures(size_t* ovector, int pairCnt);
  Capture operator[](int idx) const;
};

// Pattern manages pointers, so we will not copy it (like a unique_ptr)

class Pattern {
  pcre2_real_code_8* pattern{};
  pcre2_real_match_data_8* matchData{};

  void free();
public:
  Pattern(const Pattern&) = delete;
  Pattern& operator=(const Pattern&) = delete;
  Pattern(Pattern&& other);
  Pattern(std::string_view pat, uint32_t opts = 0);
  Pattern();
  ~Pattern();

  void set(std::string_view pat, uint32_t opts = 0);

  std::optional<Captures> match(std::string_view subject, size_t startOffset = 0, uint32_t opts = 0) const;

  cppcoro::generator<Captures> matchAll(std::string_view subject, size_t startOffset = 0, uint32_t opts = 0) const;
};

} // namespace re

