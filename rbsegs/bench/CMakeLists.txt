cmake_minimum_required(VERSION 3.12)
project(bench)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(bench main.cpp)