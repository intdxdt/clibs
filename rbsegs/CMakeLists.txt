cmake_minimum_required(VERSION 3.12)
project(rbsegs)

set(CMAKE_CXX_STANDARD 17)

add_executable(rbsegs main.cpp util.h event.h prep.h)