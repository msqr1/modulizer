
#include "HeaderProcessor.hpp"
#include "Ctre.hpp"
#include <iostream>
#include <cstring>
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
  auto res{ctre::multiline_search_all<"#include +\"(.*)(?:\\..*)\"">(text)};
  size_t leftover{};
  size_t captureSize{};
  char* dst{};
  auto it{res.begin()};
  auto next{it};
  auto end{res.end()};
  while(it != end) {
    next = std::next(it);
    dst = &*(*it).begin() - leftover;
    captureSize = (*it).get<1>().size();
    std::memcpy(dst, "import ", 7);
    std::memcpy(dst + 7, (*it).get<1>().data_unsafe(), captureSize);
    dst[7 + captureSize] = ';';
    leftover += (*it).size() - captureSize - 8;
    std::copy((*it).end(), next != end ? (*next).begin() : text.end(), (*it++).end() - leftover);
  }
  text.resize(text.size() - leftover);
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
  auto res{ctre::search_all<"export\\s*\\{\\s*\\}">(text)};
  size_t totalLen{};
  auto it{res.begin()};
  auto next{it};
  auto end{res.end()};
  while(it != end) {
    next = std::next(it);
    std::copy((*it).end(), next != end ? (*next).begin() : text.end(), (*it).begin() - totalLen);
    totalLen += (*it++).size();
  }
  text.resize(text.size() - totalLen);
  return *this;
}
void HeaderProcessor::operator>>(std::fstream &out) {
  std::cout << text << "\n";
}
