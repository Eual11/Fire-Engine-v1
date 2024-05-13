#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <uml/vec3.h>
#include <vector>

const int WINDOW_WIDTH = 640;

const int WINDOW_HEIGHT = 480;

SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;

SDL_Texture *gTexture = nullptr;

void init() {
  // sorry for lack of error checking
  SDL_Init(SDL_INIT_VIDEO);
  gWindow =
      SDL_CreateWindow("Tex", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  gRenderer = SDL_CreateRenderer(
      gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
  gTexture = IMG_LoadTexture(gRenderer, "./assets/textures/test_uv1.png");

  if (!gTexture) {
    std::cerr << "Error Occured " << SDL_GetError() << "\n";
    exit(1);
  }
}
struct triangle {
  vec3 p[3];
  triangle(std::initializer_list<vec3> vals) {
    if (vals.size() != 3)
      throw new std::invalid_argument("invalid arg");
    auto it = vals.begin();
    p[0] = *it++;
    p[1] = *it++;
    p[2] = *it;
  }
};
struct mesh {
  std::vector<triangle> tris;
};

int main(int argc, char **argv) {
  printf("Program %s running with %d args\n", argv[0], argc);
  init();

  mesh cube;
  cube.tris = {
      // FRONT FACE
      {{0, 1, 0}, {1, 1, 0}, {0, 0, 0}},
      {{1, 0, 0}, {0, 0, 0}, {1, 1, 0}},

  };

  std::vector<SDL_Vertex> verts;

  for (auto tri : cube.tris) {
    for (size_t i = 0; i < 3; i++) {
      SDL_Vertex v;
      v.position.x = 300 * tri.p[i].x + 100;
      v.position.y = WINDOW_HEIGHT - 300 * tri.p[i].y - 100;

      v.tex_coord.x = tri.p[i].x / 2;
      v.tex_coord.y = (1 - tri.p[i].y);

      v.color = {255, 255, 255, 255};
      verts.push_back(v);
    }
  }

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
        quit = true;
    }
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(gRenderer);

    SDL_RenderGeometry(gRenderer, gTexture, verts.data(), verts.size(), NULL,
                       0);
    SDL_RenderPresent(gRenderer);
  }

  return 0;
}
