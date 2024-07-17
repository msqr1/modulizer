#include "HeaderProcessor.hpp"
#include <regex>
#include <iostream>
namespace fs = std::filesystem;

HeaderProcessor& HeaderProcessor::setModuleName(const fs::path& name) {
  moduleName = name.stem();
}
void HeaderProcessor::operator<<(std::fstream& in) {
  in.seekg(0, std::ios::end);
  long size {in.tellg()};
  in.seekg(0);
  text.clear(); // Avoid copying when resizing
  if(size > text.size()) text.resize(size);
  in.read(&text[0], size);
}
HeaderProcessor& HeaderProcessor::handleAnonymousNS() {
  static const std::regex expr{"namespace\\s*({)"};
  return *this;
}

HeaderProcessor& HeaderProcessor::replaceIncludes() {
  static const std::regex expr{"#include +\"(.*)(?:\\..*)\""};
  std::string replacement;
  std::sregex_iterator it{text.begin(), text.end(), expr};
  int count{std::distance(it, std::sregex_iterator())};
  while(count --> 0) {
    replacement += "import ";
    replacement += (*it)[1].str();
    replacement += ";";
    text.replace(it->position(), (*it)[0].length(), replacement);
    replacement.clear();
    ++it;
  }
  return *this;
} 
void HeaderProcessor::operator>>(std::fstream &out){
  out << "export module " << moduleName 
      << ";\n" << text;
}
