#!/bin/bash

# You need to specify path to catch_amalgamated.cpp and add catch2 headers
# directory to gcc's include paths (if it's not already in system's include)

g++ snap_tests.cpp /path/to/catch_amalgamated.cpp -o snap_tests
