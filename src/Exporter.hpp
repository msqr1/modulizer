#pragma once
#include <iosfwd> // For std::string

struct Opts;

void addExports(std::string& content, size_t lastImportEnd, const Opts& opts);