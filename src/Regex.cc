#include "Regex.hpp"
#include "Base.hpp"
namespace modulizer::re {

void ckStatus(int status, const std::source_location& loc = std::source_location::current()) {
  // Match status OK || Compile status OK
  if(status == 0 || status == 100) return;
  char errMsg[256];
  pcre2_get_error_message(status, reinterpret_cast<PCRE2_UCHAR*>(errMsg), 256);
  exitWithErr(loc, "Regex error: {}", errMsg);
}

Pattern::Pattern(std::string_view pattern, uint32_t opts) {
  int status;
  size_t _; // Unused
  pat = pcre2_compile(reinterpret_cast<PCRE2_SPTR>(pattern.data()), pattern.length(), opts, &status, &_, nullptr);
  ckStatus(status);
  status = pcre2_jit_compile(pat, PCRE2_JIT_COMPLETE);
  ckStatus(status);
}
Pattern::~Pattern() {
  log("Dtor called");
  //pcre2_code_free(pat);
}

void match(const Pattern& pattern, std::string_view subject, uint32_t opts) {
  int status;
  pcre2_match_data* md;
  status = pcre2_jit_match(pattern.pat, reinterpret_cast<PCRE2_SPTR>(subject.data()), subject.length(), 0, opts, md, nullptr);
  ckStatus(status);
  //pcre2_match_data_free(md);
}

}