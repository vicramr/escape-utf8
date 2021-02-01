# This cmake file is here only to build winrun.c for the Appveyor job.
# It's difficult (perhaps impossible) to call cl.exe directly from
# a Powershell script that doesn't already have the proper environment
# variables, so instead I am using this cmake file to compile winrun.c.

cmake_minimum_required(VERSION 3.1)

project(winrun-ci)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /permissive- /W4 /DNDEBUG")

add_executable(winrun ../winrun.c)
