# Modulizer MVP spec
- This document outlines the Minimum Viable Product (MVP) specification for Modulizer, a command-line tool (CLI) designed to automatically convert C++ code into C++20 modules
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
    - Precedent: CLI > ```.toml``` > default except for verbose
    - CLI only: 
        -  ```[inDir]``` - Input directory
        -  ```-c --config``` - Optional path to a configuration file (```.toml```)
        -  ```-h --help``` - Print help and exit
        -  ```-V --version``` - Print version and exit
        -  ```-v --verbose``` - Enable verbose output (debugging)
        -  ```-m --merge``` - Merge declarations and definition into a single entity
    - CLI + ```.toml``` (simple/frequent use/rarely changed)
        -  ```-o --out-dir``` - output directory (defaults to input directory)
        -  ```--header-ext-regex``` - Header file extension regex
        -  ```--source-ext-regex``` - Source file extension regex
        -  ```--module-interface-ext``` - Module interface unit file extension
    - ```.toml```-only (complex/rarely changed):
        - ```openExport``` - Syntax for opening an export section
        - ```closeExport``` - Syntax for closing an export section
    - CLI options map to ```.toml``` for CLI + ```.toml``` by joining words and using camelCase (eg., ```--out-dir``` becomes ```outDir```).
    - Default values for (CLI + ```.toml```) and ```.toml```-only inputs:
        - outDir: (same as ```inDir```) [string]
        - headerExtRegex: ```"\.h(pp|xx)?"``` [string]
        - sourceExtRegex: ```"\.c(pp|c|xx)"``` [string]
        - moduleInterfaceExt: ```".cppm"``` [string]
        - openExport: ```export {\n``` [string]
        - closeExport: ```}\n``` [string]
    - Config file syntax follows [TOML](https://toml.io/en)
- Architecture diagrams: https://viewer.diagrams.net/?tags=%7B%7D&lightbox=1&highlight=0000ff&edit=_blank&layers=1&nav=1&title=Modules.html#Uhttps%3A%2F%2Fraw.githubusercontent.com%2Fmsqr1%2Fmodulizer%2Fmain%2FModules.html

## Design goals
- **Automation**: Refactor code into named modules automatically
- **Validity**: Generate correct C++20 code
- **Macro handling**: Handle preprocessor macros, especially #include guards, correctly
- **Preserve style and maintainability**: Modifications must integrate nicely with the codebase's existing style

## Code input requirement
- Valid standard C++20
- Headers must not define macros that affect other files
- Declarations in one header must have their definitions in one source with the same base name

## Maybe in the future
- Dependency analysis
- Parallel execution
- Build system integration
