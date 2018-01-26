cmake_minimum_required(VERSION 3.9)
project(mem C)

set(CMAKE_C_STANDARD 11)

add_executable(mem main.c mem.h mem.c)