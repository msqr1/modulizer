#pragma once
#include <iosfwd> // For std::string

struct Opts;

void addExports(std::string& content, const Opts& opts);