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

  HeaderProcessor& include2Import(); // FIXME: Convert include path to module name
  
  // Internal linkages handlers
  HeaderProcessor& exportNoUnnamedNS();
  HeaderProcessor& handleUnnamedUnion();
  HeaderProcessor& handleStaticEntity();

  HeaderProcessor& eraseEmptyExport();
  HeaderProcessor& write(std::fstream& out, const std::filesystem::path& p);
};