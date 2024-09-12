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
  std::filesystem::path hdrPath;
  std::fstream hdr;
  bool verbose;
public:
  HeaderProcessor(bool v) : verbose{v} {};
  HeaderProcessor& load(const std::filesystem::path& hdrPath);

  // Internal linkages handlers
  
  HeaderProcessor& exportNoUnnamedNS();
  HeaderProcessor& eraseEmptyExport();
  

  HeaderProcessor& appendSrc(const std::filesystem::path& srcPath);
  HeaderProcessor& usrInclude2Import(); // FIXME: Convert include path to module name
  // Also create the export declaration
  HeaderProcessor& sysInclude2GMF();
  HeaderProcessor& write();
};