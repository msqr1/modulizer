
#include "HeaderProcessor.hpp"
#include "Ctre.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <stack>
#include <algorithm>
#include <fstream>
namespace fs = std::filesystem;
NS::NS(bool inlined, const std::reverse_iterator<std::string::iterator>& begin, const std::reverse_iterator<std::string::iterator>& end) : inlined{inlined}, name{end.base(), begin.base()} {}
void HeaderProcessor::operator<<(std::fstream &in) {
  // Read header into string
  in.seekg(0, std::ios::end);
  long size {in.tellg()};
  in.seekg(0);
  if(size > text.capacity()) {
    text.clear();
    text.resize(size);
  }
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
HeaderProcessor& HeaderProcessor::handleUnnamedNS() {
  // Scan for unnamed namespace, see what namespace(s) it is under, and recreate the same namespace hierarchy outside the export block
  std::stack<NS> NSInsideOut;
  size_t start{};
  auto unnamedNS{ctre::multiline_search<"(inline\\s+)?namespace\\s*\\{">(text)};
  while(unnamedNS.data() != nullptr) {
    int nest{};
    int maxNest{};
    auto it{unnamedNS.begin()};
    while(it != text.begin()) {
      switch(*--it) {
        case '{':
          nest++;
          break;
        case '}':
          nest--;
      }
      if(nest > maxNest) {
        // Since we're scanning in reverse, the regex also need to be matching backward.Equivalent forward: (?:(inline)\s+)?namespace\s++(.+?)\s*{
        auto NS{ctre::multiline_search<"\\s*+(.+)\\s+ecapseman(?:\\s+(enilni))?">(std::make_reverse_iterator(it), text.rend())};
        NSInsideOut.emplace(NS.get<2>().size() > 0, NS.get<1>().begin(), NS.get<1>().end());
        maxNest = nest;
      }
    }
    // No nesting, just repair export around the unnamed NS
    if(NSInsideOut.empty()) {
      // Find matching braces
      int nestLvl{1};
      it = unnamedNS.end();
      while(nestLvl > 0) {
        switch(*it++) {
          case '{':
            nestLvl++;
            break;
          case '}':
            nestLvl--;
        }
      }
      start = it - text.begin();
      text
      .insert(unnamedNS.begin() - text.begin(), "}\n")
      .insert(start, "\nexport {\n");
      start += 12;
      std::cout << text;
    } 
    else {
      while(!NSInsideOut.empty()) {
        std::cout << NSInsideOut.top().name << " " << NSInsideOut.top().inlined << "\n";
        NSInsideOut.pop();
      }
    }
    break;
    unnamedNS = ctre::multiline_search<"(inline\\s+)?namespace\\s*\\{">(text.begin() + start, text.end());
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
