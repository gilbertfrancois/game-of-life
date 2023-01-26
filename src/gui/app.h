#ifndef GUI_H_FILE
#define GUI_H_FILE

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../lib/GameOfLifeKernel.h"
#include "config.h"

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
