
#include "HeaderProcessor.hpp"
#include "Ctre.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <fstream>
namespace fs = std::filesystem;
void HeaderProcessor::operator<<(std::fstream &in) {
  in.seekg(0, std::ios::end);
  long size {in.tellg()};
  in.seekg(0);
  text.clear(); // Avoid copying when resizing
  if(size > text.size()) text.resize(size);
  in.read(&text[0], size);
}
HeaderProcessor& HeaderProcessor::setModuleName(const fs::path& name) {
  moduleName = name.stem();
  return *this;
}
HeaderProcessor &HeaderProcessor::include2Import() {
  // Since the equivalent import is always shorter than the include, we replace and do erase-remove idiom
  auto it{text.begin()};
  size_t include{};
  for(const auto& kept : ctre::multiline_split<"#include +\"(.+)\"">(text)) {
    it = std::ranges::copy(kept, it).out;
    include = kept.get<1>().size();
    if(include > 0) {
      std::memcpy(&*it, "import ", 7);
      it += 7;
      std::memcpy(&*it, kept.get<1>().data_unsafe(), include);
      it += include;
      *it++ = ';';
    }
  }
  text.erase(it, text.end());
  return *this;
}
HeaderProcessor& HeaderProcessor::handleAnonymousNS() {
  for(const auto& match : ctre::search_all<"(?:inline\\s+)?namespace\\s*\\{">(text)) {
  
  }
  return *this;
}
HeaderProcessor& HeaderProcessor::handleStaticEntity() {
  return *this;
}
HeaderProcessor& HeaderProcessor::handleAnonymousUnion() {
  return *this;
}
HeaderProcessor &HeaderProcessor::eraseEmptyExport() {
  // Erase-remove idiom with regex
  auto it{text.begin()};
  for (const auto& kept : ctre::split<"export\\s*\\{\\s*\\}">(text)) {
    if(kept.size() > 0) it = std::ranges::copy(kept, it).out;
  }
  text.erase(it, text.end());
  return *this;
}
void HeaderProcessor::operator>>(std::fstream &out) {
  std::cout << text << "\n";
}
