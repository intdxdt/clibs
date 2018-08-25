cmake_minimum_required(VERSION 3.12)
project(rbsegs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(rbsegs
        test.cpp
        util.h
        Event.h
        prep.h
        brute.h
        seg.h
        intersects.h
        rbinter.h
        brute_force.h
)