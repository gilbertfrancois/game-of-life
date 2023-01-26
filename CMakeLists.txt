cmake_minimum_required(VERSION 3.16)
project(game-of-life)

set(CMAKE_CXX_STANDARD 14)
set(MY_PROJECT_DIR ${CMAKE_SOURCE_DIR})

# Make threading work on Linux
if(UNIX AND NOT APPLE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

include(cmake/cli.cmake)
include(cmake/gui.cmake)
