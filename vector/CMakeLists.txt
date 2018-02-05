cmake_minimum_required(VERSION 3.9)
project(vector C)

set(CMAKE_C_STANDARD 11)

add_executable(vector main.c vector.h vector.c)