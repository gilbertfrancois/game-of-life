project(game-of-life)

add_library(gol GameOfLifeKernel.cpp)

target_include_directories(gol 
    PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)
