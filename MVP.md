# MVP spec for Modulizer
- C++ modules intro: https://en.cppreference.com/w/cpp/language/modules

## Motivation (why modules?)
- Faster compilation (good for incremental build) because header files are only compiled once instead of every time it's #included
- Isolation pf preprocessor macros
- Allow merging of interface (header) and implementation (source) into one file

## Existing solution
- Refactor code by hand into named modules (best, but slow)
- Treat the header as an importable header, just change ```#include <...>``` to ```import <...>;``` (faster, but the definition of "importable" depends on the compiler)
- Using precompiled headers (just as fast as modules, but non-standard and most implementation only allow 1 per translation unit)

## Design
- A CLI tool
- Input
    - Precedent: CLI > ```.conf``` > default
    - CLI only input 
        -  ```[inDir]``` - Input directory
        -  ```-c --config [path]``` - Specify a configuration file (```.conf```)
        -  ```-h --help``` - Print help and exit
        -  ```-V --version``` - Print version and exit
    - Input available on CLI + ```.conf``` (simple/frequent use/rarely changed)
        -  ```-v --verbose``` - Activate verbose output (mostly for debugging)
        -  ```-m --merge``` - Merge source and header
        -  ```-o --out-dir [path]``` - Output directory
        -  ```--source-extension [...] ``` - Specify source extension
        -  ```--header-extension [...]``` - Specify header extension
        -  ```--module-extension [...]``` - Specify module extension
    - ```.conf``` only input (complex/rarely changed):
        - ```openExport``` - Export section start syntax
        - ```closeExport``` - Export section end syntax
- CLI to ```.conf``` map for CLI + ```.conf``` inputs: join words and use camelCase
- Default values for (CLI + ```.conf```) inputs:
    - verbose: ```false```
    - merge: ```false```
    - outDir: (same as ```inDir```)
    - headerExtension: ```hpp```
    - sourceExtension: ```cpp```
    - moduleExtension: ```cppm```
- Default values for ```.conf``` only inputs:
    - openExport: ```export {\n```
    - closeExport: ```}\n```
- Config file syntax (TOML):
```
[modulizer]
config1 = "string"
config2 = true
...
```
## Design goal
- Refactor code into named modules automatically (FAST)
- Must generate valid, modularized C++20 code.
- Must handle preprocessor macros and specifically, #include guards, correctly

## Code input requirement
- Headers must not define macros that affect other files
