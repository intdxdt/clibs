cmake_minimum_required(VERSION 3.9)
project(rtree)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -Wextra -Wpedantic -Wno-missing-braces -Wno-missing-braces")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

add_executable(rtree main.cpp util.h node.h rtree.h)