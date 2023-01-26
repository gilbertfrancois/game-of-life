#ifndef GAMEOFLIFE_GAMEOFLIFEGUI_H
#define GAMEOFLIFE_GAMEOFLIFEGUI_H

#include "config.h"
#include "../lib/GameOfLifeKernel.h"

int parse_arguments(std::vector<std::string> args, Config *config);

int main(int argc, char *argv[]);

#endif
