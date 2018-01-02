cmake_minimum_required(VERSION 3.9)
project(mbr C)

set(CMAKE_C_STANDARD 11)
add_executable(mbr main.c mbr.h util.h)
target_link_libraries(mbr m)