#pragma once
#include <iosfwd> // For std::string

struct Opts;
struct File;

void modularize(File& file, const Opts& opts);