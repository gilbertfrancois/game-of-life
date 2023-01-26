#include "app.h"
#include <SDL_pixels.h>
#include <iostream>
#include <ostream>

App::App(Config config_) : config(config_) {

    steps_per_sec = 10;
    timer_fps = 0;
    time = 0;
    prev_time = 0;
    fps = 0;
    frame_count = 0;
    running = false;
    init_video();
    kernel =
        new GameOfLifeKernel(config.rows, config.cols, config.with_threads);
}

App::~App() {
    delete kernel;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void App::init_video() {
    auto status = SDL_Init(SDL_INIT_VIDEO);

    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    std::cout << "Current display mode: "
              << SDL_GetPixelFormatName(mode.format) << std::endl;
    std::cout << "Current display res: " << mode.w << ", " << mode.h
              << std::endl;

    if (config.mode_fullscreen) {
        config.display_w = mode.w;
        config.display_h = mode.h;
    } else {
        config.display_w = mode.w / 2;
        config.display_h = mode.h / 2;
    }
    config.rows = (int)(config.display_h / config.zoom_factor);
    config.cols = (int)(config.display_w / config.zoom_factor);

    window = SDL_CreateWindow("Game of Life", 0, 0, config.display_w, config.display_h, 0);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (config.mode_fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        update_window_size();
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    texture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                          SDL_TEXTUREACCESS_TARGET, config.cols, config.rows);
}

void App::update() {
    kernel->timestep();
    update_window_size();
    update_events();
}

void App::update_window_size() {
    SDL_GetWindowSize(window, &config.display_w, &config.display_h);
}

void App::update_events() {
    SDL_Event event;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    // Handle keyboard events
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            running = false;
        if (keystates[SDL_SCANCODE_ESCAPE])
            running = false;
        if (keystates[SDL_SCANCODE_Q])
            running = false;
    }
}

void App::draw() {
    draw_cells();
    draw_progress_bar();
    SDL_RenderPresent(renderer);
}

void App::draw_cells() {
    SDL_SetRenderTarget(renderer, texture);
    // Clear render target.
    SDL_SetRenderDrawColor(renderer, 0, 0, 100, 255);
    SDL_RenderClear(renderer);
    // Draw pixels.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int row = 0; row < config.rows; row++) {
        for (int col = 0; col < config.cols; col++) {
            int value = kernel->get_xt_at(row, col);
            if (value == 1)
                SDL_RenderDrawPoint(renderer, col, row);
        }
    }
    // Copy texture to screen.
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void App::draw_progress_bar() {
    const int y = (int)(config.display_h - 1);
    const int x0 = 0;
    const int x1 = (int)(progress * (float)config.display_w);
    SDL_RenderDrawLine(renderer, x0, y, x1, y);
}

void App::tick_one_sec() {
    time = SDL_GetTicks();
    if (time >= (prev_time + 1000)) {
        prev_time = time;
        fps = frame_count;
        frame_count = 0;
    }
}

void App::limit_fps() {
    frame_count++;
    timer_fps = SDL_GetTicks() - time;
    if (timer_fps < (1000 / steps_per_sec)) {
        SDL_Delay((1000 / steps_per_sec) - timer_fps);
    }
}

void App::run() {
    running = true;
    for (int step = 0; step < config.n_steps; step++) {
        if (!running)
            break;

        tick_one_sec();

        progress = (float)step / config.n_steps;
        update();
        draw();

        limit_fps();
    }
}
