cmake_minimum_required(VERSION 3.11)
project(geom)

set(CMAKE_CXX_STANDARD 17)

add_executable(geom main.cpp point.h geom.h line_mono.h line.h util.h  geometry.h)