# This file is part of SnapConfig - a free (as in freedom) cofig reader library

cmake_minimum_required(VERSION 3.26)
project(SnapConfig_tests)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Path to catch_amalgamated.cpp. Don't forget to set this variable to your actual path
set(catch2_amalgamated_source "/usr/include/catch2/catch_amalgamated.cpp")

if(NOT EXISTS "${catch2_amalgamated_source}")
    message(FATAL_ERROR "Source Catch2 amalgamated file '${catch2_amalgamated_source}' doesn't exist. Did you forget to set related variable in CMakeLists.txt?")
endif()

add_executable(snap_tests snap_tests.cpp ${catch2_amalgamated_source})
