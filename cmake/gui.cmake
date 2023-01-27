find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
message(STATUS ${SDL2_INCLUDE_DIRS})
message(STATUS ${SDL2_LIBRARIES})

add_executable(game-of-life-gui
    src/gui/main.cpp
    src/gui/app.cpp
    src/lib/GameOfLifeKernel.cpp
    )

# Link SDL2::Main, SDL2::Image and SDL2::GFX to our project
target_link_libraries(game-of-life-gui ${SDL2_LIBRARIES})
