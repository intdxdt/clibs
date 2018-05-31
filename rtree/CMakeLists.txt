cmake_minimum_required(VERSION 3.9)
project(rtree_cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(rtree_cpp
        main.cpp
        rtree.h
        node.h
        util.h
        )