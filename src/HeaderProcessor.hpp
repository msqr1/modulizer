#pragma once
#include <filesystem>
class HeaderProcessor {
  bool verbose;
  std::string moduleName;
  std::string text;
public:
  HeaderProcessor(bool verbose) : verbose{verbose} {};
  void operator<<(std::fstream& in);
  HeaderProcessor& setModuleName(const std::filesystem::path& name);
  HeaderProcessor& include2Import();
  HeaderProcessor& handleAnonymousNS();
  HeaderProcessor& handleInternalLinkages();
  HeaderProcessor& eraseEmptyExport();
  void operator>>(std::fstream& out);
};