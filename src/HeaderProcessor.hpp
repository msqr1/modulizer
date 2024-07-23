#pragma once
#include <filesystem>
struct NS {
  bool inlined;
  std::string_view name;
  NS(bool inlined, const std::reverse_iterator<std::string::iterator>& begin, const std::reverse_iterator<std::string::iterator>& end);
};
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
  // Does not work for nested unnamed namespace (idk why you would do that though)
  HeaderProcessor& handleUnnamedNS();
  // NOTE: Anonymous unions can be in namespaces but can't be exported (the bad part)
  HeaderProcessor& handleAnonymousUnion();
  HeaderProcessor& handleStaticEntity();
  HeaderProcessor& eraseEmptyExport();
  void operator>>(std::fstream& out);
};