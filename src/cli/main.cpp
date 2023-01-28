#include <chrono>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "main.h"

// https://www.linuxquestions.org/questions/programming-9/get-width-height-of-a-terminal-window-in-c-810739/
//
using namespace std::chrono_literals;


void get_terminal_size(Config *config) {
    int arg_rows = config->rows;
    int arg_cols = config->cols;
    int term_rows = 0;
    int term_cols = 0;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    term_cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    term_rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#elifdef TIOCGSIZE
    struct ttysize ts;
    ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
    term_cols = ts.ts_cols;
    term_rows = ts.ts_lines;
#elif defined(TIOCGWINSZ)
    struct winsize ts;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
    term_cols = ts.ws_col;
    term_rows = ts.ws_row;
#endif
    // Don't exceed the terminal size, even if the user has requested.
    config->rows =
        (arg_rows >= term_rows || arg_rows == 1) ? term_rows : arg_rows;
    config->cols =
        (arg_cols >= term_cols || arg_cols == 1) ? term_cols : arg_cols;
    if (config->rows >= term_rows) {
        // Subtract one line from the domain for the status line.
        config->rows -= 1;
    }
    // Don't allow a illegal size.
    if (config->rows <= 0) {
        config->rows = 29;
    }
    if (config->cols <= 0) {
        config->cols = 80;
    }
}

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
                << "   --without-threads     : compute single threaded."
                << std::endl;
            std::cout
                << "   --with-threads        : compute multi-threaded."
                << std::endl;
            std::cout << "   -h, --help            : info and help message."
                      << std::endl;
            exit(0);
        } else if (*i == "--width") {
            config->cols = stoi(*++i);
        } else if (*i == "--height") {
            config->rows = stoi(*++i);
        } else if (*i == "--steps") {
            config->n_steps = stoi(*++i);
        } else if (*i == "--without-threads") {
            config->with_threads = false;
        } else if (*i == "--with-threads") {
            config->with_threads = true;
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    // Initialize default values
    Config config;
    config.cols = 1;
    config.rows = 1;
    config.n_steps = 1000;
    config.with_threads = true;
    // Parse arguments
    std::vector<std::string> args(argv + 1, argv + argc);
    parse_arguments(args, &config);
    // Get the default terminal size.
    std::cout << config.cols << std::endl;
    get_terminal_size(&config);
    std::cout << config.cols << std::endl;
    // Init the kernel.
    GameOfLifeKernel *kernel =
        new GameOfLifeKernel(config.rows, config.cols, config.with_threads);
    int n_threads = kernel->get_n_threads();
    int n_cpus = kernel->get_n_cpus();
    // Allow the user to read the domain slicing in the terminal window.
    std::this_thread::sleep_for(1s);
    // Game loop.
    for (auto i = 0; i < config.n_steps; i++) {
        // VT100 compatible escape codes to clear the screen.
        std::cout << "\033[H\033[J";
        // Print the current state of the domain.
        /* kernel->printDomain(); */
        std::cout << kernel->to_string(); 
        // Print a status line.
        std::cout << "[ cpus: " << n_cpus << " ]-";
        std::cout << "[ threads: " << n_threads << " ]-";
        std::cout << "[ width: " << config.cols << " ]-";
        std::cout << "[ height: " << config.rows << " ]-";
        std::cout << "[ step: " << i << " / " << config.n_steps - 1 << " ] ";
        std::flush(std::cout);
        // Go one timestep forward.
        kernel->timestep();
        std::this_thread::sleep_for(0.1s);
    }
    // Cleanup
    delete kernel;

    exit(0);
}
