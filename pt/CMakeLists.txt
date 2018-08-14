cmake_minimum_required(VERSION 3.10)
project(pt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
add_executable(pt main.cpp pt.h)