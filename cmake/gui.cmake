find_package(SDL2 CONFIG REQUIRED)

# Use this with older cmake versions
# include_directories(${SDL2_INCLUDE_DIRS})

add_executable(game-of-life-gui
    src/gui/main.cpp
    src/gui/app.cpp
    src/lib/GameOfLifeKernel.cpp
    )

target_link_libraries(game-of-life-gui
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

# Use this with older cmake versions
# target_link_libraries(game-of-life-gui ${SDL2_LIBRARIES})
