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
#include <iostream>
#include "main.h"
#include "app.h"

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
            std::cout << "game-of-life-gui" << std::endl;
            std::cout
                << "   --steps <number>      : number of steps, default = 1000."
                << std::endl;
            std::cout
                << "   --bt <number>      : boundary type: 0=const, 1=periodic, 2=mirror, default=1."
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
        } else if (*i == "--steps") {
            config->n_steps = stoi(*++i);
        } else if (*i == "--bt") {
            config->boundary_type = stoi(*++i);
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
    config.boundary_type = BOUNDARY_PERIODIC;
    std::vector<std::string> args(argv + 1, argv + argc);
    parse_arguments(args, &config);
    App *app = new App(config);
    app->run();
    delete app;
    return 0;
}
