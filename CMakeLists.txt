cmake_minimum_required(VERSION 3.0)
project(opsufile_project VERSION 0.1.0)
enable_testing()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "DEBUG")
endif()

add_subdirectory("dep")
add_subdirectory("lib")
