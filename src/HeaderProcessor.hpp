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
  std::string txt;
  std::filesystem::path hdrPath;
  std::fstream hdr;
public:
  HeaderProcessor& load(const std::filesystem::path& hdrPath);

  // Internal linkages handlers
  HeaderProcessor& exportNoUnnamedNS();
  HeaderProcessor& eraseEmptyExport();
  HeaderProcessor& unexportStaticUnion();
  HeaderProcessor& appendSrc(const std::filesystem::path& srcPath);
  HeaderProcessor& usrInclude2Import(); // FIXME: Convert include path to module name
  // Also create the export declaration
  HeaderProcessor& sysInclude2GMF();
  HeaderProcessor& write();
};