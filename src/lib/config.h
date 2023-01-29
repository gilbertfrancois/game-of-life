#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int rows;
    int cols;
    int n_steps;
    int boundary_type;
    int display_w;
    int display_h;
    int zoom_factor;
    bool with_threads;
    bool mode_fullscreen;
} Config;

#endif
