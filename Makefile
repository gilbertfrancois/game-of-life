CPP_FLAGS          := --std=c++17 -O2 -Wall
CPP_DEBUG_FLAGS    := --std=c++17 -g -Wall

INCLUDE_PATHS      := -I/usr/local/include
LIB_PATHS          := -L/usr/local/lib
SOURCES_LIB        := src/lib/*.cpp
SOURCES_CLI        := ${SOURCES_LIB} src/cli/*.cpp
SOURCES_GUI        := ${SOURCES_LIB} src/gui/*.cpp
GUI_COMPILER_FLAGS := `sdl2-config --cflags`
GUI_LINKER_FLAGS   := `sdl2-config --libs`

default: pre cli cli-debug

pre:
	mkdir -p build/

all: default

cli:
	c++ ${CPP_FLAGS} ${SOURCES_CLI} ${INCLUDE_PATHS} -o build/game-of-life-cli

cli-debug:
	c++ ${CPP_DEBUG_FLAGS} ${SOURCES_CLI} ${INCLUDE_PATHS} -o build/game-of-life-cli-debug

clean:
	rm -rf build/*

.PHONY: default
