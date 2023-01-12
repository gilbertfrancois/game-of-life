CPP_FLAGS=--std=c++17 -O2 -Wall
CPP_DEBUG_FLAGS=--std=c++17 -g -Wall

# INCLUDE_PATHS=-I/opt/homebrew/include
# LIB_PATHS=-L/opt/homebrew/lib
# STATIC_LIB_PREFIX=/opt/homebrew/lib
INCLUDE_PATHS=-I/usr/local/include
LIB_PATHS=-L/usr/local/lib
SOURCES_CLI=src/GameOfLifeKernel.cpp src/main.cpp
COMPILER_FLAGS = `sdl2-config --cflags`
LINKER_FLAGS = `sdl2-config --libs`

cli:
	mkdir -p build/
	c++ ${CPP_FLAGS} ${SOURCES_CLI} -o build/game-of-life-cli

cli-debug:
	mkdir -p build/
	c++ ${CPP_DEBUG_FLAGS} ${SOURCES_CLI} -o build/game-of-life-cli-debug


