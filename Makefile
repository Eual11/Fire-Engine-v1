SRC = ./src/test4.cpp
DEMO0_SRC = ./demos/demo0.cpp
DEMO1_SRC = ./demos/demo1.cpp
# SRC:= ./src/tex_test.cpp
OBJ_NAME = e

CFLAGS:= -g  -Wall -Wextra -Wpedantic -std=c++17
LIB_PATH:= -L"${C_LIBRARY_PATH}"

LINKER_FLAGS:= -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lumlvec2 -lumlvec3 -lumlmat4x4 -lumlmat3x3 -lumltransform -lumlutils
CC := g++ 

all: ${SRC}
	${CC} ${SRC} ${CFLAGS} ${LIB_PATH} ${LINKER_FLAGS} -o ./build/${OBJ_NAME}
demo0:
		${CC} ${DEMO0_SRC} ${CFLAGS} ${LIB_PATH} ${LINKER_FLAGS} -o ./build/$@
demo1:
		${CC} ${DEMO1_SRC} ${CFLAGS} ${LIB_PATH} ${LINKER_FLAGS} -o ./build/$@
