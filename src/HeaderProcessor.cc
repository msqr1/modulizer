#include "HeaderProcessor.hpp"
#include "Logging.h"
#include "Ctre.hpp"
#include <cstring>
#include <vector>
#include <print>
#include <algorithm>
#include <fstream>
namespace fs = std::filesystem;
NS::NS(size_t open, size_t close) : open{open}, close{close} {}
NS::NS() : open{0}, close{0} {}
size_t rtnSize(const char*, size_t size) {
  return size;
}
HeaderProcessor& HeaderProcessor::load(const fs::path& hdrPath) {
  this->hdrPath = hdrPath;
  hdr.open(hdrPath);
  if(!hdr.good()) logAndExit("Unable to load header {}", hdrPath.c_str());
  size_t size{fs::file_size(hdrPath)};
  text.clear();
  text.resize_and_overwrite(size, rtnSize);
  hdr.read(text.data(), size);
  return *this;
}
HeaderProcessor& HeaderProcessor::sysInclude2GMF() {
  auto it{text.rend()};
  for(const auto& kept : ctre::multiline_split<R"((#\s*include\s*<.+>\s*\n))">(text)) {
    if(kept.get<1>().data_unsafe() == nullptr) continue;
    std::rotate(std::make_reverse_iterator(kept.get<1>().end()),std::make_reverse_iterator(kept.end()), it);
    it -= kept.get<1>().size();
  }
  std::format_to(std::inserter(text, it.base()), "export module {};\n", hdrPath.stem().c_str());
  return *this;
}
HeaderProcessor& HeaderProcessor::usrInclude2Import() {
  // Since the equivalent import is always shorter than the include, we replace and do erase-remove idiom
  auto it{text.begin()};
  for(const auto& kept : ctre::multiline_split<R"(#\s*include\s*\"(.+)\")">(text)) {
    it = std::ranges::copy(kept, it).out;
    if(kept.get<1>().data_unsafe() != nullptr) 
      it = std::format_to(it, "import {};", kept.get<1>().to_view());
  }
  text.erase(it, text.end());
  return *this;
}
HeaderProcessor& HeaderProcessor::exportNoUnnamedNS() {
  static constexpr std::string_view OPEN_EXPORT{"export {"};
  static constexpr std::string_view CLOSE_EXPORT{"}"};
  static constexpr size_t EXPORT_LEN{OPEN_EXPORT.size() + CLOSE_EXPORT.size()};
  // Use as a stack but still need to iterate over
  std::vector<NS> stack;
  std::string::iterator it;
  std::string::iterator begin;
  NS self;
  NS* parent;
  bool unnamed{};
  auto search{ctre::search<R"((?:inline\s+)?namespace(.*?)\{)">};
  stack.emplace_back(0, text.size());
  while(!stack.empty()) {
    begin = text.begin();
    parent = &stack.back();
    auto NS = search(begin + parent->open, begin + parent->close);
    if(NS.data() == nullptr) {
      text.insert(parent->close, CLOSE_EXPORT).insert(parent->open, OPEN_EXPORT);
      stack.pop_back();
      for(auto& [open, close] : stack) {
        open += EXPORT_LEN;
        close += EXPORT_LEN;
      }
      continue;
    }
    int nest{1};
    it = NS.end();
    self.open = it - begin;
    while(nest) {
      switch(*++it) {
        case '{':
          nest++;
          break;
        case '}':
          nest--;
      }
    }
    self.close = it - begin;
    unnamed = NS.get<1>().to_view().find_first_not_of(" \n\t\v\f\r") == std::string::npos;
    text.insert(NS.begin() - begin, CLOSE_EXPORT).insert(parent->open, OPEN_EXPORT);
    self.open += EXPORT_LEN;
    self.close += EXPORT_LEN;
    for(auto& [open, close] : stack) {
      open += EXPORT_LEN;
      close += EXPORT_LEN;
    }
    parent->open = self.close + 1;
    if(!unnamed) stack.emplace_back(self);
  }
  return *this;
}
HeaderProcessor& HeaderProcessor::eraseEmptyExport() {
  // Erase-remove idiom with regex
  auto it{text.begin()};
  for (const auto& kept : ctre::split<R"(export\s*\{\s*\})">(text)) {
    if(kept.size() > 0) it = std::ranges::copy(kept, it).out;
  }
  text.erase(it, text.end());
  return *this;
}
HeaderProcessor& HeaderProcessor::write() {
  /*hdr
  .seekp(0)
  .write("module;\n", 8)
  .write(text.data(), text.size());
  */
  hdr.close();
  std::print("{}", text);
  return *this;
}
HeaderProcessor& HeaderProcessor::appendSrc(const fs::path& srcPath) {
  if(std::ifstream src{srcPath}) {
    size_t size{fs::file_size(srcPath)};
    text.resize_and_overwrite(size + text.size(), rtnSize);
    src.read(text.data() + text.size(), size);
  }
  else {
    logAndExit("Unable to append source {}", srcPath.c_str());
  }
  //fs::remove(srcPath);
  return *this;
}