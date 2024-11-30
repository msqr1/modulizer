#pragma once
#include "Regex.hpp"
#include <iosfwd> // For std::string

struct Opts;

void addExports(std::string& content, const Opts& opts);