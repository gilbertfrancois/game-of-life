cmake_minimum_required(VERSION 3.22)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")

project(game-of-life)

set(CMAKE_CXX_STANDARD 17)
set(MY_PROJECT_DIR ${CMAKE_SOURCE_DIR})
add_compile_options(-Wall -Werror) 

# Make threading work on Linux
if(UNIX AND NOT APPLE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

#===============================================================================
# Game Of Life library
#===============================================================================
add_subdirectory(lib/gol)
add_subdirectory(src/cli)
add_subdirectory(src/gui)
