project(game-of-life)

add_executable(game-of-life-cli main.cpp)

target_link_libraries(game-of-life-cli 
    PRIVATE
    gol
)
