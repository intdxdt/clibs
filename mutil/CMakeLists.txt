cmake_minimum_required(VERSION 3.10)
project(mutil)

set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

set(CMAKE_CXX_STANDARD 17)

add_executable(mutil main.cpp)