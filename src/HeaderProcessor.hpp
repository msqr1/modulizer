#include <fstream>
#include <filesystem>
class HeaderProcessor {
  std::string moduleName;
  std::string text;
public:
  void operator<<(std::fstream& in);
  HeaderProcessor& setModuleName(const fs::path& name);
  HeaderProcessor& replaceIncludes();
  HeaderProcessor& handleAnonymousNS();
  void operator>>(std::fstream& out);
};