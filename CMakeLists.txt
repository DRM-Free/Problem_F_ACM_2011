cmake_minimum_required(VERSION 3.10)
project(assessment VERSION 1.0.0)

set(CMAKE_BUILD_TYPE
    Release
    CACHE STRING "build type" FORCE)

add_compile_options(
  $<$<CONFIG:Debug>:-fsanitize=address,undefined>
  -Wall
  -Wextra
  -pedantic
  -Wconversion
  -Wold-style-cast
  -Wsign-conversion
  -Werror=return-type)
add_link_options($<$<CONFIG:Debug>:-fsanitize=address,undefined>)

add_executable(assessment main.cpp)

target_compile_features(assessment PRIVATE cxx_std_17)
