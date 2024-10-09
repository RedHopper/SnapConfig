# This file is part of SnapConfig - a free (as in freedom) cofig reader library

cmake_minimum_required(VERSION 3.26)

project(SnapConfig_example)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(example example.cpp)
