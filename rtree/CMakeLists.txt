cmake_minimum_required(VERSION 3.9)
project(rtree_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(rtree_cpp
        main.cpp
        rtree.h
        util.h node.h)