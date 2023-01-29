#ifndef GAMEOFLIFE_GAMEOFLIFECLI_H
#define GAMEOFLIFE_GAMEOFLIFECLI_H

#include "../lib/GameOfLifeKernel.h"
#include "../lib/config.h"

void get_terminal_size(int *rows, int *cols);

int parse_arguments(std::vector<std::string> args, Config *config);

int main(int argc, char *argv[]);

#endif
