cmake_minimum_required(VERSION 3.0)
project(untitled7 C)

set(CMAKE_C_STANDARD 99)

add_executable(untitled7 main.c loader.h linuxload.c cmake-build-debug/elfload.c)
