cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")

project(game-of-life)

set(CMAKE_CXX_STANDARD 17)
set(MY_PROJECT_DIR ${CMAKE_SOURCE_DIR})

# Make threading work on Linux
if(UNIX AND NOT APPLE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

include(cmake/cli.cmake)
include(cmake/gui.cmake)
