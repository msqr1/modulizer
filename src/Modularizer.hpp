#pragma once
#include <cstddef> // For size_t

struct Opts;
struct File;

// Return the end of the last import statement
size_t modularize(File& file, const Opts& opts);