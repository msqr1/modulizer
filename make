#!/bin/bash
# shellcheck disable=SC2086
# Clang 20 is required

# Do ./make 1 for release config

if [ ! -d 3rdParty ]; then
  mkdir 3rdParty
fi
cd 3rdParty &&
if [ ! -d fast_io ]; then
  mkdir fast_io
  cd fast_io &&
  git init &&
  git fetch https://github.com/cppfastio/fast_io 788cb9810f0ca881d15fe594b0dd1144901d14d8 --depth 1 &&
  git checkout 788cb9810f0ca881d15fe594b0dd1144901d14d8 &&
  # Go to include/fast_io_hosted/posix_error_scatter/impl.h and add #include <errno.h> under #if __has_include(<errno.h>)
  cd ..
fi
if [ ! -f Toml++.hpp ]; then
  wget https://raw.githubusercontent.com/marzer/tomlplusplus/refs/tags/v3.4.0/toml.hpp -O Toml++.hpp
fi
cd ../src &&
flags="-std=c++23 -stdlib=libc++"

if [ "$1" = "1" ]; then
  flags+="-Ofast -flto"
fi

files="Base.cc ArgProcessor.cc FileOp.cc Merger.cc Exporter.cc Modularizer.cc Main.cc"
clang++ $flags -o modulizer



