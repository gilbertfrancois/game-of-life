cmake_minimum_required(VERSION 3.22)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")

project(game-of-life)

set(CMAKE_CXX_STANDARD 17)
set(MY_PROJECT_DIR ${CMAKE_SOURCE_DIR})

# Make threading work on Linux
if(UNIX AND NOT APPLE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

option(BUILD_CLI_APP "Build GameOfLife terminal application." ON)
if(BUILD_CLI_APP)
    include(cmake/cli.cmake)
endif()

option(BUILD_GUI_APP "Build GameOfLife GUI application." ON)
if(BUILD_GUI_APP)
    include(cmake/gui.cmake)
endif()
