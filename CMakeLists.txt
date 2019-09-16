cmake_minimum_required(VERSION 3.1)
# This version number was chosen mostly arbitrarily.

project(escape_utf8)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Note that CMake versions prior to 3.10 don't add standard switches to
# the compiler invocation for MSVC: https://stackoverflow.com/a/46876754
# In addition, versions of MSVC prior to VS 2015 Update 3 don't have them at all.
# Reference: https://devblogs.microsoft.com/cppblog/standards-version-switches-in-the-compiler/
# Just something to keep in mind; shouldn't break any compiler that would otherwise
# build successfully, as (I think) MSVC 2015 will support c++14 features by default.

# In case we're using a single-generation generator (such as Make)
# we need to set a default configuration. See here for more info on Debug/Release configs:
# https://cmake.org/pipermail/cmake/2006-February/008371.html
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Next we will set compiler options. Because different compilers have
# different command-line arg syntax (most notably MSVC) we will set the
# options on a case-by-case basis.
# Values for the variable CMAKE_CXX_COMPILER_ID are given here:
# https://cmake.org/cmake/help/v3.1/variable/CMAKE_LANG_COMPILER_ID.html
# Some inspiration taken from this SO post: https://stackoverflow.com/q/45955272
# and from this CMakeLists.txt file: https://github.com/leela-zero/leela-zero/blob/next/CMakeLists.txt
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    # Don't need to add any flags for debug. Optimizations are turned off by default: https://docs.microsoft.com/en-us/cpp/build/reference/od-disable-debug?view=vs-2019
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /O2 /GL /Gw") # Except /DNDEBUG, these flags all turn on optimizations.
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    # These compilers should all use a similar syntax.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
# For any other compilers, we're not setting any flags. This should help ensure that the program will at least compile with other compilers.
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

add_executable(escape src/main.cpp src/parseargs.cpp src/StreamPair.cpp src/business_logic.cpp)

# Here I'm just treating the test as another target.
add_executable(runtest test/test.cpp src/parseargs.cpp src/StreamPair.cpp src/business_logic.cpp test/unit_tests_parseargs.cpp)
