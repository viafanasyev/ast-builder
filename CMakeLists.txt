cmake_minimum_required(VERSION 3.16)
project(ast-builder)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-Wall -Wextra -pedantic -Werror -Wfloat-equal)


add_executable(
        ast-builder
        src/main.cpp)

add_executable(
        tests
        test/main.cpp
        test/testlib.h
        test/testlib.cpp)
