#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <uml/mat4x4.h>
#include <uml/vec2.h>
#include <uml/vec3.h>
#include <uml/vec4.h>
#include <vector>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;

mat4x4 MakeClipMatrix(float aspectRatio, float fov, float zNear, float zFar);
mat4x4 MakeRotX(float theta);
mat4x4 MakeRotY(float theta);
mat4x4 MakeRotZ(float theta);

void init();
void close_program();

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

  init();
  printf("Progam %s started with %d args\n", argv[0], argc);

  mesh cube;
  cube.tris = {
      // FRONT FACE
      {{0, 1, 0}, {1, 1, 0}, {0, 0, 0}},
      {{1, 0, 0}, {0, 0, 0}, {1, 1, 0}},

      // BACK
      {{1, 0, 1}, {1, 1, 1}, {0, 1, 1}},
      {{1, 0, 1}, {0, 1, 1}, {0, 0, 1}},

      // LFFT
      {{0, 1, 1}, {0, 1, 0}, {0, 0, 1}},
      {{0, 1, 0}, {0, 0, 0}, {0, 0, 1}},

      // RIGHT

      {{1, 1, 0}, {1, 1, 1}, {1, 0, 1}},
      {{1, 0, 1}, {1, 0, 0}, {1, 1, 0}},

      // TOP
      {{0, 1, 0}, {0, 1, 1}, {1, 1, 1}},
      {{1, 1, 1}, {1, 1, 0}, {0, 1, 0}},

      // Bottom
      {{0, 0, 0}, {1, 0, 1}, {0, 0, 1}},
      {{1, 0, 1}, {0, 0, 0}, {1, 0, 0}},

  };
  const char *names[] = {"FRONT", "BACK", "LEFT", "RIGHT", "TOP", "Bottom"};
  float i = 0;
  for (auto &tri : cube.tris) {
    vec3 normal = Cross(tri.p[1] - tri.p[0], tri.p[2] - tri.p[0]);
    normal.normalize();
    printf("%s:  (%f, %f, %f)\n", names[static_cast<int>(i)], normal.x,
           normal.y, normal.z);
    i += 0.5;
  }
  // parameters for the perspective projections
  //
  float fov = M_PI / 2;
  float aspectRatio = static_cast<float>(WINDOW_HEIGHT) / WINDOW_WIDTH;
  float zFar = 10;
  float zNear = 2;
  float fBank = 0.0;
  float fYaw = 0.0;

  for (auto &tri : cube.tris) {
    for (int i = 0; i < 3; i++) {
      // slightly pushing out cube (count it as obj to world transform)

      /* tri.p[i].z += 3; */
    }
  }
  mat4x4 clipMatrix = MakeClipMatrix(aspectRatio, fov, zNear, zFar);

  std::vector<SDL_Vertex> projectedMesh;

  bool quit = false;

  SDL_Event e;
  while (!quit) {

    projectedMesh.clear();
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }
    for (auto tri : cube.tris) {

      // some goofy rotation
      vec3 normal = Cross(tri.p[1] - tri.p[0], tri.p[2] - tri.p[0]);

      mat4x4 ry = MakeRotY(fYaw);
      mat4x4 rz = MakeRotX(fBank);
      for (int i = 0; i < 3; i++) {
        vec4 v = {tri.p[i], 1};
        v = v * ry;
        tri.p[i].x = v.x;
        tri.p[i].y = v.y;
        tri.p[i].z = v.z;
      }
      for (int i = 0; i < 3; i++) {
        // slightly pushing out cube (count it as obj to world transform)

        tri.p[i].z += 3;
      }
      // projecting to screen space
      for (int i = 0; i < 3; i++) {
        vec3 v = tri.p[i];
        vec4 projectVec = vec4{v, 1} * clipMatrix;
        // perspective divide
        projectVec.x /= projectVec.w;
        projectVec.y /= projectVec.w;
        projectVec.z /= projectVec.w;

        SDL_Color cols[] = {
            {217, 186, 218, 255}, {180, 198, 231, 255},
            {255, 250, 205, 255}, {227, 185, 182, 255}, // yellow
            {155, 20, 188, 255},                        // purple
            {53, 166, 224, 255}                         // blue

        };
        SDL_Vertex ver;
        ver.position.x = (projectVec.x + 1) * 0.5 * WINDOW_WIDTH;
        ver.position.y = (projectVec.y + 1) * 0.5 * WINDOW_HEIGHT;
        if (normal == vec3(0, 0, -1)) {
          ver.color = cols[1];
        }
        if (normal == vec3(0, 0, 1)) {
          ver.color = cols[2];
        }
        if (normal == vec3(0, 1, 0)) {
          ver.color = cols[3];
        }
        if (normal == vec3(0, -1, 0)) {
          ver.color = cols[4];
        }
        if (normal == vec3(0, 0, -1)) {
          ver.color = cols[4];
        }
        if (normal == vec3(0, 0, 1)) {
          ver.color = cols[5];
        }

        projectedMesh.push_back(ver);
      }
    }

    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xff);

    SDL_RenderGeometry(gRenderer, NULL, projectedMesh.data(),
                       projectedMesh.size(), NULL, 0);
    SDL_RenderPresent(gRenderer);
    SDL_RenderClear(gRenderer);
    fYaw += 0.01;
    fBank += 0.02;
  }
  close_program();
  return 0;
}
void init() {
  if (SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "Error Occured: %s\n", SDL_GetError());
    exit(1);
  }

  gWindow = SDL_CreateWindow("Burn!", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                             WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  if (!gWindow) {
    fprintf(stderr, "Error Occured: %s\n", SDL_GetError());
    exit(1);
  }

  gRenderer = SDL_CreateRenderer(
      gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!gRenderer) {
    fprintf(stderr, "Error Occured: %s\n", SDL_GetError());
    exit(1);
  }
}
void close_program() {
  SDL_DestroyWindow(gWindow);
  gWindow = nullptr;
  SDL_DestroyRenderer(gRenderer);
  gRenderer = nullptr;

  SDL_Quit();

  printf("Closed Gracefully\n");
}
mat4x4 MakeClipMatrix(float aspectRatio, float fov, float zNear, float zFar) {

  mat4x4 m{0};
  float zoom = 1.0 / tan(fov / 2);
  float lamba = zFar / (zFar - zNear);
  m[0][0] = aspectRatio * zoom;
  m[1][1] = zoom;
  m[2][2] = lamba;
  m[2][3] = 1;
  m[3][2] = -zNear * lamba;

  return m;
}

mat4x4 MakeRotX(float theta) {
  float st = sin(theta);
  float ct = cos(theta);
  mat4x4 m{

      1,   0,  0, 0, 0, ct, st, 0, 0,
      -st, ct, 0, 0, 0, 0,  1

  };
  return m;
}
mat4x4 MakeRotY(float theta) {

  float st = sin(theta);
  float ct = cos(theta);
  mat4x4 m{

      ct, 0, -st, 0, 0, 1, 0, 0, st, 0, ct, 0, 0, 0, 0, 1};

  return m;
}

mat4x4 MakeRotZ(float theta) {
  float st = sin(theta);
  float ct = cos(theta);

  mat4x4 m{ct, st, 0, 0, -st, ct, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  return m;
}
