cmake_minimum_required(VERSION 3.8)
project(twiddle)

set(CMAKE_CXX_STANDARD 17)

set(
        SOURCE_FILES
        tests.cpp
        twiddle.h
        ../catch/catch.h
)
add_executable(twiddle ${SOURCE_FILES})