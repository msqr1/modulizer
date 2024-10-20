# Modulizer MVP spec
- This document outlines the Minimum Viable Product (MVP) specification for Modulizer, a command-line tool (CLI) designed to automatically convert C++ code into C++20 modules
- C++ modules intro: https://en.cppreference.com/w/cpp/language/modules

## Motivation (why modules?)
- **Faster compilation**: Good for incremental builds because header files are only compiled once instead of every time it's #included
- **Macro isolation**: Prevent unintended side effects from preprocessor macros
- **Simplified code organization**: Merging interface and implementation into a single file

## Existing solution 
- **Refactor code manually into named modules**: Best control, but this approach is painful and error-prone
- **Treat the header as an importable header (only change ```#include <...>``` to ```import <...>;```)**: faster to do, but the definition of "importable" depends on the compiler
- **Using precompiled headers**: More mature support, but non-standard and most implementation only allow 1 per translation unit

## Design
- A CLI tool
- Input & options
    - Precedent: CLI > ```.conf``` > default
    - CLI only: 
        -  ```[inDir]``` - Input directory
        -  ```-c --config``` - Optional path to a configuration file (```.conf```)
        -  ```-h --help``` - Print help and exit
        -  ```-V --version``` - Print version and exit
    - CLI + ```.conf``` (simple/frequent use/rarely changed)
        -  ```-v --verbose``` - Enable verbose output (debugging)
        -  ```-m --merge``` - Merge source and header files into a single module
        -  ```-o --out-dir``` - Specify output directory (defaults to input directory)
        -  ```--header-extension``` - Comma-separated list of header file extensions (e.g., ```.hpp,.h```)
        -  ```--source-extension``` - Comma-separated list of source file extensions (e.g., ```.cpp,.cc```)
        -  ```--module-extension``` - Module extension(s) (e.g., ```.cppm,.ixx```)
    - ```.conf```-only (complex/rarely changed):
        - ```openExport``` - Syntax for opening an export section
        - ```closeExport``` - Syntax for closing an export section
- CLI options map to ```.conf``` for CLI + ```.conf``` by joining words and using camelCase (eg., ```--out-dir``` --> ```outDir```).
- Default values for (CLI + ```.conf```) and ```.conf```-only inputs:
    - verbose: ```false``` [boolean]
    - merge: ```false``` [boolean]
    - outDir: (same as ```inDir```) [string]
    - headerExtension: ```[".hpp, ".h"]``` [string array]
    - sourceExtension: ```[".cpp", ".cc"]``` [string array]
    - moduleExtension: ```".cppm"``` [string]
    - openExport: ```export {\n``` [string]
    - closeExport: ```}\n``` [string]
- Config file syntax follow [TOML](https://toml.io/en)

## Design goals
- **Automation**: Refactor code into named modules automatically
- **Validity**: Generate correct C++20 code
- **Macro handling**: Handle preprocessor macros, especially #include guards, correctly
- **Preserve style and maintainability**: Modifications integrate nicely with the codebase's existing style

## Code input requirement
- Headers must not define macros that affect other files

## Future features
- Dependency analysis
