#pragma once
#include <filesystem>
struct NS {
  size_t open;
  size_t close;
  NS(size_t open, size_t close);
  NS();
};
class HeaderProcessor {
  std::string text;
  bool verbose{};
  int indent{};
public:
  HeaderProcessor(bool v, int i) : verbose{v}, indent{i} {};
  HeaderProcessor& read(std::fstream& in, const std::filesystem::path& p);
  // Export everything that is not a NS
  HeaderProcessor& exportNoNS();
  // FIXME: Replace illegal export characters like hyphen
  HeaderProcessor& include2Import();
  HeaderProcessor& handleAnonymousUnion();
  HeaderProcessor& handleStaticEntity();
  HeaderProcessor& eraseEmptyExport();
  HeaderProcessor& write(std::fstream& out, const std::filesystem::path& p);
};