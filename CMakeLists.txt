cmake_minimum_required(VERSION 3.1)
# This version number was chosen mostly arbitrarily.

project(escape-utf8)

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED on)

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

add_executable(escape src/main.cpp)

# Here I'm just treating the test as another target.
add_executable(runtest test/test.cpp)

