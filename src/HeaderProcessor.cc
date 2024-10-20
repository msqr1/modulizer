#include "HeaderProcessor.hpp"
#include "Logging.h"
#include "../3rd-party/Ctre.hpp"
#include <cstring>
#include <vector>
#include <print>
#include <algorithm>
#include <fstream>
namespace fs = std::filesystem;

static constexpr std::string_view OPEN_EXPORT{"export {"};
static constexpr std::string_view CLOSE_EXPORT{"}"};
static constexpr size_t EXPORT_LEN{OPEN_EXPORT.size() + CLOSE_EXPORT.size()};

size_t rtnSize(const char*, size_t size) {
  return size;
}
void matchBrace(size_t& pos, const std::string& str) {
  int nest{1};
  while(nest) {
    switch(str[++pos]) {
    case '{':
      nest++;
      break;
    case '}':
      nest--;
    }
  }
}
NS::NS(size_t open, size_t close) : open{open}, close{close} {}
NS::NS() : open{0}, close{0} {}
HeaderProcessor& HeaderProcessor::load(const fs::path& hdrPath) {
  this->hdrPath = hdrPath;
  hdr.open(hdrPath);
  if(!hdr.good()) logAndExit("Unable to load header {}", hdrPath.c_str());
  size_t size{fs::file_size(hdrPath)};
  txt.clear();
  txt.resize_and_overwrite(size, rtnSize);
  hdr.read(txt.data(), size);
  return *this;
}
HeaderProcessor& HeaderProcessor::sysInclude2GMF() {
  auto it{txt.rend()};
  for(const auto& kept : ctre::multiline_split<R"((#\s*include\s*<.+>\s*\n))">(txt)) {
    if(kept.get<1>().data_unsafe() == nullptr) continue;
    std::rotate(std::make_reverse_iterator(kept.get<1>().end()),std::make_reverse_iterator(kept.end()), it);
    it -= kept.get<1>().size();
  }
  std::format_to(std::inserter(txt, it.base()), "export module {};\n", hdrPath.stem().c_str());
  return *this;
}
HeaderProcessor& HeaderProcessor::usrInclude2Import() {
  // Since the equivalent import is always shorter than the include, we replace and do erase-remove idiom
  auto it{txt.begin()};
  for(const auto& kept : ctre::multiline_split<R"(#\s*include\s*\"(.+)\")">(txt)) {
    it = std::ranges::copy(kept, it).out;
    if(kept.get<1>().data_unsafe() != nullptr)
      it = std::format_to(it, "import {};", kept.get<1>().to_view());
  }
  txt.erase(it, txt.end());
  return *this;
}
HeaderProcessor& HeaderProcessor::exportNoUnnamedNS() {
  // Use as a stack but still need to iterate over
  std::vector<NS> stack;
  size_t pos{};
  std::string::iterator begin;
  NS self;
  NS* parent;
  bool unnamed{};
  int nest{};
  auto search{ctre::search<R"((?:inline\s+)?namespace(.*?)\{)">};
  stack.emplace_back(0, txt.size());
  while(!stack.empty()) {
    // Prevent iterator validation
    begin = txt.begin();
    parent = &stack.back();
    auto NS = search(begin + parent->open, begin + parent->close);
    if(NS.data() == nullptr) {
      txt.insert(parent->close, CLOSE_EXPORT).insert(parent->open, OPEN_EXPORT);
      stack.pop_back();
      for(auto& [open, close] : stack) {
        open += EXPORT_LEN;
        close += EXPORT_LEN;
      }
    }
    else {
      pos = NS.end() - begin;
      self.open = pos;
      matchBrace(pos, txt);
      self.close = pos;
      unnamed = NS.get<1>().to_view().find_first_not_of(" \n\t\v\f\r") == std::string::npos;
      txt.insert(NS.begin() - begin, CLOSE_EXPORT).insert(parent->open, OPEN_EXPORT);
      self.open += EXPORT_LEN;
      self.close += EXPORT_LEN;
      for(auto& [open, close] : stack) {
        open += EXPORT_LEN;
        close += EXPORT_LEN;
      }
      parent->open = self.close + 1;
      if(!unnamed) stack.emplace_back(self);
    }
  }
  return *this;
}
HeaderProcessor& HeaderProcessor::eraseEmptyExport() {
  // Erase-remove idiom with regex
  auto it{txt.begin()};
  for(const auto& kept : ctre::split<R"(export\s*\{\s*\})">(txt)) {
    if(kept.size() > 0) it = std::ranges::copy(kept, it).out;
  }
  txt.erase(it, txt.end());
  return *this;
}
HeaderProcessor& HeaderProcessor::unexportStaticUnion() {
  auto search{ctre::search<R"((static\s+)?union\s*\{)">};
  std::string::iterator begin{txt.begin()};
  size_t start{};
  size_t end{};
  size_t pos{};
  auto res{search(txt.begin(), txt.end())};
  while(res.data() != nullptr) {
    pos = res.end() - begin - 1;
    matchBrace(pos, txt);
    end = txt.find(';', pos) + 1;
    if(res.get<1>().data_unsafe() != nullptr ||
    std::string_view{txt.data() + pos, end - pos}.contains("static")) {
      start = res.begin() - begin;
      txt
      .insert(end, OPEN_EXPORT)
      .insert(start, CLOSE_EXPORT);
      end += EXPORT_LEN;
    }
    start = end;
    begin = txt.begin();
    res = search(begin + start, txt.end());
  }
  return *this;
}
HeaderProcessor& HeaderProcessor::write()
{
  /*hdr
  .seekp(0)
  .write("module;\n", 8)
  .write(txt.data(), txt.size());
  */
  hdr.close();
  std::print("{}", txt);
  return *this;
}
HeaderProcessor& HeaderProcessor::appendSrc(const fs::path& srcPath) {
  if(std::ifstream src{srcPath}) {
    size_t size{fs::file_size(srcPath)};
    txt.resize_and_overwrite(size + txt.size(), rtnSize);
    src.read(txt.data() + txt.size(), size);
  }
  else {
    logAndExit("Unable to append source {}", srcPath.c_str());
  }
  //fs::remove(srcPath);
  return *this;
}