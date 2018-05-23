cmake_minimum_required(VERSION 3.10)
project(mbr)
set(CMAKE_CXX_FLAGS "-g -O0 -Wall -Wextra -Wno-missing-braces -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

set(CMAKE_CXX_STANDARD 17)

add_executable(mbr main.cpp mbr.h )