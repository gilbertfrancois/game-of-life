CPP_FLAGS=-O2 -Wall
CPP_DEBUG_FLAGS=-g -Wall

INCLUDE_PATHS=-I/opt/homebrew/include
LIB_PATHS=-L/opt/homebrew/lib
STATIC_LIB_PREFIX=/opt/homebrew/lib
# INCLUDE_PATHS=-I/usr/local/include
# LIB_PATHS=-L/usr/local/lib

all:
	mkdir -p build/
	cpp ${CPP_FLAGS} src/c/pong.c ${INCLUDE_PATHS} ${LIB_PATHS} -lSDL2 -o build/gol

debug:
	mkdir -p build/
	cpp ${CPP_DEBUG_FLAGS} src/c/pong.c ${INCLUDE_PATHS} ${LIB_PATHS} -lSDL2 -o build/gol

