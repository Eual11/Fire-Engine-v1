#include "../include/FireEngine/premitives.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <algorithm>
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
  gTexture = IMG_LoadTexture(gRenderer, "./assets/textures/dirt.png");

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

  FireEngine::Object3D cube;
  cube.LoadFromObj("./assets/objects/texcube.obj");
  std::sort(cube.triangles.begin(), cube.triangles.end(),
            [](const FireEngine::triangle &a, const FireEngine::triangle &b) {
              float z1 =
                  (a.verticies[0].position.z + a.verticies[1].position.z +
                   a.verticies[2].position.z) /
                  3;

              float z2 =
                  (b.verticies[0].position.z + b.verticies[1].position.z +
                   b.verticies[2].position.z) /
                  3;

              return z1 > z2;
            });

  std::vector<SDL_Vertex> verts;
  bool t = false;
  for (auto tri : cube.triangles) {
    for (size_t i = 0; i < 3; i++) {
      SDL_Vertex v;
      v.position.x = 200 * tri.verticies[i].position.x + 300;
      v.position.y = WINDOW_HEIGHT - 200 * tri.verticies[i].position.y - 250;

      v.tex_coord.x = 1 - tri.verticies[i].uv.x;
      v.tex_coord.y = 1 - tri.verticies[i].uv.y;
      /* FE_CLAMP(v.tex_coord.x, 0, 1); */
      /* FE_CLAMP(v.tex_coord.y, 0, 1); */

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
