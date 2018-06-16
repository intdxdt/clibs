cmake_minimum_required(VERSION 3.11)
project(geom)

set(CMAKE_CXX_STANDARD 17)

add_executable(geom main.cpp point.h geom.h mono.h line.h util.h  geometry.h ring.h coords.h wkt.h)