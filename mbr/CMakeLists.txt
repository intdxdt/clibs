cmake_minimum_required(VERSION 3.9)
project(mbr C)

set(CMAKE_C_STANDARD 11)
add_executable(mbr mbr.h util.h test.h test_mbr.c)
target_link_libraries(mbr m)