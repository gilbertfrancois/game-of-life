cmake_minimum_required(VERSION 3.25)
project(game-of-life)

set(CMAKE_CXX_STANDARD 17)
set(MY_PROJECT_DIR ${CMAKE_SOURCE_DIR})

include(cmake/cli.cmake)
include(cmake/gui.cmake)
