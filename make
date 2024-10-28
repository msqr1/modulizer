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
  cd ..
fi
if [ ! -f Toml++.hpp ]; then
  wget https://raw.githubusercontent.com/marzer/tomlplusplus/refs/tags/v3.4.0/toml.hpp -O Toml++.hpp
fi
cd ../src &&
cxxFlags="-std=c++23 -stdlib=libc++ -fprebuilt-module-path= -fimplicit-module-maps -fimplicit-modules"
moduleFlags="-fmodule-output -fexperimental-modules-reduced-bmi"

if [ "$1" = "1" ]; then
  cxxFlags+="-Ofast -flto"
fi

files="Base.ccm ArgProcessor.ccm FileOp.ccm Merger.ccm Exporter.ccm Modularizer.ccm"

clang++ $cxxFlags $moduleFlags -c $files
clang++ $cxxFlags -o modulizer Main.cc ${files//.ccm/.o}

rm -f *.pcm *.o



