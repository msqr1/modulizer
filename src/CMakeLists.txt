cmake_minimum_required(VERSION 3.25)
project(modulizer)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" inSrc)
if(inSrc)
  message(FATAL_ERROR "In-source build is unsupported")
endif()
# Setup the 3rdParty folder
execute_process(
  WORKING_DIRECTORY ../3rdParty 
  COMMAND cmake -P Setup.cmake
)
add_executable(modulizer Base.cc ArgProcessor.cc FileOp.cc Merger.cc Exporter.cc Modularizer.cc Main.cc)
target_compile_features(modulizer PUBLIC cxx_std_23)
add_subdirectory(../3rdParty/fmt "${CMAKE_BINARY_DIR}/fmt")
target_link_libraries(modulizer fmt::fmt)
add_subdirectory(../3rdParty/toml++ "${CMAKE_BINARY_DIR}/toml++")
target_link_libraries(modulizer tomlplusplus::tomlplusplus)



 