#pragma once
#include <filesystem>
class HeaderProcessor {
  std::string moduleName;
  std::string text;
  bool verbose{};
  int indent{};
public:
  HeaderProcessor(bool v, int i) : verbose{v}, indent{i} {};
  void operator<<(std::fstream& in);
  HeaderProcessor& setModuleName(const std::filesystem::path& name);
  // FIXME: Replace illegal export characters like hyphen
  HeaderProcessor& include2Import();
  // FIXME: Make this works for nested anonymous NS, too. 
  HeaderProcessor& handleAnonymousNS();
  // NOTE: Anonymous unions can be in namespaces but can't be exported (the bad part)
  HeaderProcessor& handleAnonymousUnion();
  HeaderProcessor& handleStaticEntity();
  HeaderProcessor& eraseEmptyExport();
  void operator>>(std::fstream& out);
};