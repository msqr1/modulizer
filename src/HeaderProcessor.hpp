#pragma once
#include <filesystem>
#include <fstream>
struct NS {
  size_t open;
  size_t close;
  NS(size_t open, size_t close);
  NS();
};
class HeaderProcessor {
  std::string text;
  std::fstream hdr;
  bool verbose{};
public:
  HeaderProcessor(bool v) : verbose{v} {};
  HeaderProcessor& load(const std::filesystem::path& p);

  HeaderProcessor& include2Import(); // FIXME: Convert include path to module name
  
  // Internal linkages handlers
  HeaderProcessor& exportNoUnnamedNS();
  HeaderProcessor& handleUnnamedUnion();
  HeaderProcessor& handleStaticEntity();

  HeaderProcessor& eraseEmptyExport();
  HeaderProcessor& write(const std::filesystem::path& p);
  HeaderProcessor& appendSrc(const std::filesystem::path& p);
};