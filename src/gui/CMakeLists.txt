project(game-of-life)

find_package(SDL2 CONFIG REQUIRED)

add_executable(game-of-life-gui
    main.cpp
    app.cpp
    )

target_link_libraries(game-of-life-gui
    PRIVATE
    gol
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
