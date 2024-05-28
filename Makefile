SRC = ./src/test3.cpp
DEMO_SRC = ./demos/demo0.cpp
# SRC:= ./src/tex_test.cpp
OBJ_NAME = e

CFLAGS:= -g  -Wall -Wextra -Wpedantic -std=c++17
LIB_PATH:= -L"${C_LIBRARY_PATH}"

LINKER_FLAGS:= -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lumlvec2 -lumlvec3 -lumlmat4x4 -lumlmat3x3 -lumltransform -lumlutils
CC := g++ 

all: ${SRC}
	${CC} ${SRC} ${CFLAGS} ${LIB_PATH} ${LINKER_FLAGS} -o ./build/${OBJ_NAME}
demo:
		${CC} ${DEMO_SRC} ${CFLAGS} ${LIB_PATH} ${LINKER_FLAGS} -o ./build/$@
