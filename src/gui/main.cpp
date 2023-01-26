#include "main.h"
#include "app.h"
#include <iostream>

int parse_arguments(std::vector<std::string> args, Config *config) {
    for (auto i = args.begin(); i != args.end(); ++i) {
        if (*i == "-h" || *i == "--help") {
            std::cout
                << "-----------------------------------------------------------"
                   "---------------------"
                << std::endl;
            std::cout << "Game of Life" << std::endl;
            std::cout << "(C) 2022, Gilbert Francois Duivesteijn" << std::endl;
            std::cout
                << "-----------------------------------------------------------"
                   "---------------------"
                << std::endl;
            std::cout << "game-of-life-cli" << std::endl;
            std::cout << "   --width <number>      : width of the domain, "
                         "default is current terminal width."
                      << std::endl;
            std::cout << "   --height <number>     : height of the domain, "
                         "default is current terminal height."
                      << std::endl;
            std::cout
                << "   --steps <number>      : number of steps, default = 1000."
                << std::endl;
            std::cout
                << "   --zoom <number>       : zoom factor, default = 1."
                << std::endl;
            std::cout
                << "   --without-threads     : compute single threaded."
                << std::endl;
            std::cout
                << "   --with-threads        : compute multi-threaded."
                << std::endl;
            std::cout
                << "   --fullscreen          : display full screen."
                << std::endl;
            std::cout 
                << "   -h, --help            : info and help message."
                << std::endl;
            exit(0);
        } else if (*i == "--width") {
            config->cols = stoi(*++i);
        } else if (*i == "--height") {
            config->rows = stoi(*++i);
        } else if (*i == "--steps") {
            config->n_steps = stoi(*++i);
        } else if (*i == "--zoom") {
            config->zoom_factor = stoi(*++i);
        } else if (*i == "--without-threads") {
            config->with_threads = false;
        } else if (*i == "--with-threads") {
            config->with_threads = true;
        } else if (*i == "--fullscreen") {
            config->mode_fullscreen = true;
        }
    }
    return 0;
}


int main(int argc, char *argv[]) {
    Config config;
    config.cols = 320;
    config.rows = 240;
    config.with_threads = true;
    config.mode_fullscreen = false;
    config.zoom_factor = 1;
    config.n_steps = 1000;

    std::vector<std::string> args(argv + 1, argv + argc);
    parse_arguments(args, &config);
    App *app = new App(config);
    app->run();
    delete app;
    return 0;
}
