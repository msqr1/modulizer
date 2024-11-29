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

add_executable(modulizer Base.cc Regex.cc ArgProcessor.cc FileOp.cc Exporter.cc Modularizer.cc Main.cc) 
target_compile_features(modulizer PUBLIC cxx_std_23)
target_compile_options(modulizer PUBLIC -Wno-deprecated-literal-operator)

# 3rd-party headers rarely changes
set(3rdPartyHeaders Generator.hpp toml++/include/toml++/toml.hpp fmt/include/fmt/format.h)
list(TRANSFORM 3rdPartyHeaders PREPEND "../3rdParty/")

# Project headers that should not be changed frequently
set(stableHeaders Base.hpp)
foreach(header IN LISTS 3rdPartyHeaders stableHeaders)
  target_precompile_headers(modulizer PUBLIC ${header})
endforeach()

set(BUILD_SHARED_LIBS on)

add_subdirectory(../3rdParty/fmt "${CMAKE_BINARY_DIR}/fmt")
target_link_libraries(modulizer fmt::fmt)

target_compile_definitions(modulizer PUBLIC TOML_EXCEPTIONS=0)
target_compile_definitions(modulizer PUBLIC TOML_ENABLE_FORMATTERS=0)
add_subdirectory(../3rdParty/toml++ "${CMAKE_BINARY_DIR}/toml++")
target_link_libraries(modulizer tomlplusplus::tomlplusplus)

set(PCRE2_BUILD_TESTS off)
set(PCRE2_BUILD_PCRE2GREP off)
set(BUILD_STATIC_LIBS off)
set(PCRE2_SUPPORT_JIT on)
add_subdirectory(../3rdParty/pcre2  "${CMAKE_BINARY_DIR}/pcre2")
target_link_libraries(modulizer pcre2-8-shared)



 