# Modulizer MVP spec
- This document outlines the Minimum Viable Product (MVP) specification for Modulizer, a command-line tool (CLI) designed to automatically convert C++ with #include to modules
- C++ modules reference: https://en.cppreference.com/w/cpp/language/modules
- C++ modules intro: https://vector-of-bool.github.io/2019/03/10/modules-1.html

## Motivation (why modules?)
- **Faster compilation**: Good for incremental builds because header files are only compiled once instead of every time it's #included
- **Macro isolation**: Prevent unintended side effects from preprocessor macros
- **Simplified code organization**: Can merge interface and implementation into a single file

## Existing solution 
- **Refactor code manually into named modules**: Best control, but this approach is painful and error-prone
- **Treat the header as an importable header (only change ```#include <...>``` to ```import <...>;```)**: faster to do, but the definition of "importable" depends on the compiler
- **Using precompiled headers**: More mature support, but non-standard and most implementation only allow 1 per translation unit

## Design
- A CLI tool
- Input & options:
    - CLI only (flags, one-time settings): 
        -  ```-c --config``` - Path to configuration file (```.toml```), default to ```modulizer.toml```
        -  ```-h --help``` - Print help and exit
        -  ```-v --version``` - Print version and exit
    - ```.toml```-only (modularization settings):
        - ```inDir``` - Input directory (required)
        - ```outDir``` - Output directory (required)
        - ```verbose``` - Enable verbose output (debugging)
        - ```duplicateIncludeMode``` - Set the duplicate include prevention mode used by the project. (```0```: include guards, ```1```: ```#pragma once```)
        - ```headerExtRegex``` - Header file extension regex
        - ```sourceExtRegex``` - Source file extension regex
        - ```moduleInterfaceExt``` - Module interface unit file extension
        - ```openExport``` - Syntax for opening an export section
        - ```closeExport``` - Syntax for closing an export section
    - Default values for ```.toml```-only settings:
        - verbose: ```false``` [boolean]
        - duplicateIncludeMode: ```0``` [int]
        - headerExtRegex: ```"\.h(pp|xx)?"``` [string]
        - sourceExtRegex: ```"\.c(pp|c|xx)"``` [string]
        - moduleInterfaceExt: ```".cppm"``` [string]
        - openExport: ```export {\n``` [string]
        - closeExport: ```}\n``` [string]
    - Config file syntax follows [TOML](https://toml.io/en)
    
## Design goals
- **Automation**: Refactor code into named modules automatically
- **Validity**: Generate correct C++20 code
- **Macro handling**: Handle preprocessor macros, especially #include guards, correctly
- **Preserve style and maintainability**: Modifications must integrate nicely with the codebase's existing style

## Code input requirement
- Valid standard C++20
- Headers must not define macros that affect other files,

## Maybe in the future (post MVP)
- Merging source and header
- Dependency analysis
- Parallel execution
- Build system integration
