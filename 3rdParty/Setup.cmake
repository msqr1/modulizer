cmake_minimum_required(VERSION 3.25)

function(gitClone repo hash outDir)
  string(CONCAT url "https://github.com/" ${repo})
  file(MAKE_DIRECTORY ${outDir})
  execute_process(
    WORKING_DIRECTORY ${outDir} 
    COMMAND git init
  )
  execute_process(
    WORKING_DIRECTORY ${outDir} 
    COMMAND git fetch --depth 1 ${url} ${hash}
  )
  execute_process(
    WORKING_DIRECTORY ${outDir} 
    COMMAND git checkout ${hash}
  )
endfunction(gitClone repo hash outDir)

if(NOT EXISTS fmt)
  gitClone(fmtlib/fmt 0c9fce2ffefecfdce794e1859584e25877b7b592 fmt)
endif()

if(NOT EXISTS toml++)
  gitClone(marzer/tomlplusplus 30172438cee64926dc41fdd9c11fb3ba5b2ba9de toml++)
endif()
