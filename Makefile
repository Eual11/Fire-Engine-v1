SRC = ./src/test.cpp
# SRC:= ./src/tex_test.cpp
OBJ_NAME = e

CFLAGS:= -g -Wall -Wextra -Wpedantic -std=c++17
LIB_PATH:= -L"${C_LIBRARY_PATH}"

LINKER_FLAGS:= -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lumlvec2 -lumlvec3 -lumlmat4x4 -lumlmat3x3
#-lmingw32 -lSDL2main -lSDL2 -lSDL2_image
CC := g++ 

all: ${SRC}
	${CC} ${SRC} ${CFLAGS} ${LIB_PATH} ${LINKER_FLAGS} -o ./build/${OBJ_NAME}
