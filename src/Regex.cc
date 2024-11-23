#include "Regex.hpp"
#include "Base.hpp"
#define PCRE2_CODE_UNIT_WIDTH 8
#include "../3rdParty/pcre2/src/pcre2.h.generic"
#include "../3rdParty/Generator.hpp"

namespace modulizer::re {

void ckErr(int status, const std::source_location& loc = std::source_location::current()) {
  // Code inside this range is OK
  if(status > -2 && status < 101) return;
  char errMsg[256];
  pcre2_get_error_message(status, reinterpret_cast<PCRE2_UCHAR*>(errMsg), 256);
  exitWithErr(loc, "Regex error: {}", errMsg);
}

Capture::Capture(size_t start, size_t end) : start{start}, end{end} {}

Captures::Captures(size_t *ovector): ovector{ovector} {}
Capture Captures::operator[](int idx) {
  // ovector comes in pairs of (start, end), so multiply by 2 to get correct index
  idx *= 2;
  return Capture{ovector[idx], ovector[idx + 1]};
}

Pattern::Pattern(): pattern{nullptr}, matchData{nullptr} {}
Pattern::Pattern(Pattern&& other): pattern{other.pattern}, matchData{other.matchData} {
  other.pattern = nullptr;
  other.matchData = nullptr;
}
Pattern::Pattern(std::string_view pat, uint32_t opts) {
  set(pat, opts);
}
Pattern::~Pattern() {
  free();
}
void Pattern::free() {
  pcre2_code_free(pattern);
  pcre2_match_data_free(matchData);
}
void Pattern::set(std::string_view pat, uint32_t opts) {
  // Free old pattern
  free();
  int status;
  size_t _; // Unused
  pattern = pcre2_compile(reinterpret_cast<PCRE2_SPTR>(pat.data()), pat.length(), opts, &status, &_, nullptr);
  ckErr(status);
  status = pcre2_jit_compile(pattern, PCRE2_JIT_COMPLETE);
  ckErr(status);
  matchData = pcre2_match_data_create_from_pattern(pattern, nullptr);
  if(matchData == nullptr) exitWithErr("Regex error: Unable to allocate memory for match");
}
std::optional<Captures> Pattern::match(std::string_view subject, uint32_t opts, size_t startOffset) {
  int count{pcre2_jit_match(pattern, reinterpret_cast<PCRE2_SPTR>(subject.data()), subject.length(), startOffset, opts, matchData, nullptr)};
  ckErr(count);
  if(count < 1) return std::nullopt;

  return pcre2_get_ovector_pointer(matchData);
}
cppcoro::generator<Captures> Pattern::matchAll(std::string_view subject, uint32_t opts, size_t startOffset) {
  while(std::optional<Captures> maybeCaptures{match(subject, opts, startOffset)}) {
    Captures& captures{*maybeCaptures};
    startOffset = captures.ovector[1];
    co_yield captures;
  }
  co_return;
}

} // namespace modulizer::re