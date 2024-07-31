#include "HeaderProcessor.hpp"
#include "Ctre.hpp"
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
namespace fs = std::filesystem;
NS::NS(size_t open, size_t close) : open{open}, close{close} {}
NS::NS() : open{0}, close{0} {}
HeaderProcessor& HeaderProcessor::read(std::fstream& in, const fs::path& p) {
  size_t size {fs::file_size(p)};
  text.clear();
  text.resize(size);
  in.read(text.data(), size);
  return *this;
}
HeaderProcessor& HeaderProcessor::exportNoNS() {
  static constexpr std::string_view OPEN_EXPORT{"export {"};
  static constexpr std::string_view CLOSE_EXPORT{"}"};
  static constexpr size_t EXPORT_LEN{OPEN_EXPORT.size() + CLOSE_EXPORT.size()};
  // Use as a stack but still need to iterate over
  std::vector<NS> stackLike;
  std::string::iterator it;
  std::string::iterator begin;
  NS self;
  NS* parent;
  bool unnamed{};
  auto search{ctre::search<R"((?:inline\s+)?namespace(.*?)\{)">};
  stackLike.emplace_back(0, text.size());
  while(!stackLike.empty()) {
    begin = text.begin();
    parent = &stackLike.back();
    auto NS = search(begin + parent->open, begin + parent->close);
    if(NS.data() == nullptr) {
      text.insert(parent->close, CLOSE_EXPORT).insert(parent->open, OPEN_EXPORT);
      stackLike.pop_back();
      for(auto& [open, close] : stackLike) {
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
    for(auto& [open, close] : stackLike) {
      open += EXPORT_LEN;
      close += EXPORT_LEN;
    }
    parent->open = self.close + 1;
    if(!unnamed) stackLike.emplace_back(self);
  }
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
HeaderProcessor& HeaderProcessor::handleStaticEntity() {
  return *this;
}
HeaderProcessor& HeaderProcessor::handleAnonymousUnion() {
  return *this;
}
HeaderProcessor &HeaderProcessor::eraseEmptyExport() {
  // Erase-remove idiom with regex
  auto it{text.begin()};
  for (const auto& kept : ctre::split<R"(export\s*\{\s*\})">(text)) {
    if(kept.size() > 0) it = std::ranges::copy(kept, it).out;
  }
  text.erase(it, text.end());
  return *this;
}
HeaderProcessor &HeaderProcessor::write(std::fstream& out, const fs::path& p) {
  //fs::resize_file(p, 0);
  std::cout << text << "\n";
  return *this;
}
