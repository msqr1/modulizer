
#include "HeaderProcessor.hpp"
#include <regex>
#include <iostream>
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
  static const std::regex expr{"#include +\"(.*)(?:\\..*)\""};
  std::string replacement{"import "};
  std::sregex_iterator it{text.begin(), text.end(), expr};
  long count{std::distance(it, std::sregex_iterator())};
  while(count --> 0) {
    replacement += (*it)[1];
    replacement += ";";
    text.replace(it->position(), it->length(), replacement);
    replacement.resize(7);
    ++it;
  }
  return *this;
}
HeaderProcessor& HeaderProcessor::handleAnonymousNS() {
  static const std::regex expr{"namespace\\s*\\{"};
  std::sregex_iterator it{text.begin(), text.end(), expr};
  long count{std::distance(it, std::sregex_iterator())};
  while(count --> 0) {
    int braceLvl{1};
    long current{it->position() + it->length()};
    while(braceLvl > 0) {
      switch(text[current]) {
        case '{':
          braceLvl++;
          break;
        case '}':
          braceLvl--;
      }
      current++;
    }
    text.insert(current, "\nexport\n {").insert(it->position(), "}\n");
  }
  return *this;
}
HeaderProcessor &HeaderProcessor::handleInternalLinkages() {
  return *this;
}
HeaderProcessor &HeaderProcessor::eraseEmptyExport() {
  static const std::regex expr{"export\\s*\\{\\s*\\}"};
  std::string::iterator begin{text.begin()};
  std::string::iterator end{text.end()};
  std::sregex_iterator it{begin, end, expr};
  long count{std::distance(it, std::sregex_iterator())};
  long totalLen = 0;
  while(count --> 0) {
    std::copy(begin + it->position() + it->length(), 
      count == 0 ? begin + std::next(it)->position() : end, 
    begin + it->position() - totalLen);
    totalLen += it++->length();
  }
  text.erase(end - totalLen, end);
  return *this;
}
void HeaderProcessor::operator>>(std::fstream &out) {
  std::cout << "export module " << moduleName 
      << ";\n" << text << "\n";
}
