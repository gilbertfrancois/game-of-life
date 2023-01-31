//   Copyright 2023 Gilbert Francois Duivesteijn
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
#ifndef GAMEOFLIFE_GUI_APP_H
#define GAMEOFLIFE_GUI_APP_H

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "gol/GameOfLifeKernel.h"
#include "gol/config.h"

class App {
  public:
    App(Config config);
    virtual ~App();
    void run();

  private:
    Config config;
    bool running;
    int display_w;
    int display_h;
    Uint32 steps_per_sec;
    Uint32 timer_fps;
    Uint32 time;
    Uint32 prev_time;
    Uint32 fps;
    Uint32 frame_count;
    float progress;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    GameOfLifeKernel *kernel;

    void init_video();
    void update();
    void update_window_size();
    void update_events();
    void draw();
    void draw_cells();
    void draw_progress_bar();

    void tick_one_sec();
    void limit_fps();

};

#endif
