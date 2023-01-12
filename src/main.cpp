#include <chrono>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>
#include <cstdio>
#include <sys/ioctl.h>
#include <unistd.h>

// https://www.linuxquestions.org/questions/programming-9/get-width-height-of-a-terminal-window-in-c-810739/
//
using namespace std::chrono_literals;

#include "main.h"

int main(int argc, char **argv) {
    int cols1 = 80;
    int lines = 24;
#ifdef TIOCGSIZE
    struct ttysize ts;
    ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
    cols1 = ts.ts_cols;
    lines = ts.ts_lines;
#elif defined(TIOCGWINSZ)
    struct winsize ts;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
    cols1 = ts.ws_col;
    lines = ts.ws_row;
#endif /* TIOCGSIZE */
    std::cout << cols1 << " x " << lines << std::endl;
    return 0;
    // Default values
    int rows = 25;
    int cols = 80;
    int n_steps = 100;
    // Parse arguments
    std::vector<std::string> args(argv + 1, argv + argc);
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
            std::cout << "gol-cli" << std::endl;
            std::cout << "   --width <number>      : width of the domain, "
                         "default = 80."
                      << std::endl;
            std::cout << "   --height <number>     : height of the domain, "
                         "default = 25."
                      << std::endl;
            std::cout
                << "   --steps <number>      : number of steps, default = 1000."
                << std::endl;
            std::cout << "   -h, --help            : info and help message."
                      << std::endl;
            return 0;
        } else if (*i == "--width") {
            cols = stoi(*++i);
        } else if (*i == "--height") {
            rows = stoi(*++i);
        } else if (*i == "--steps") {
            n_steps = stoi(*++i);
        }
    }
    GameOfLifeKernel *kernel = new GameOfLifeKernel(rows, cols, true);
    // Allow the user to read the domain slicing.
    std::this_thread::sleep_for(1s);
    // Game loop.
    for (auto i = 0; i < n_steps; i++) {
        std::cout << "\033[H\033[J";
        kernel->printDomain();
        std::cout << "Timestep " << i << " / " << n_steps - 1 << std::endl;
        kernel->timeStep();
        std::this_thread::sleep_for(1s);
    }
    delete kernel;

    return 0;
}
