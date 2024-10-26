# modulizer
C++20 modularizer, convert #include code into using C++ module

# Requirements
- Valid standard C++20
- Headers must not define macros that affect other files
- Declarations in one header must have their definitions in one source with the same base name

# Dependencies
- [CTRE](https://github.com/hanickadot/compile-time-regular-expressions) commit 129d535 modded
- [Toml++](https://github.com/marzer/tomlplusplus) 3.4.0