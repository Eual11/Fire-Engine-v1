#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <cmath>
#include <iostream>
#include <uml/mat4x4.h>
#include <uml/transform.h>
#include <uml/vec2.h>
#include <uml/vec3.h>
#include <uml/vec4.h>
#include <vector>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;
SDL_Texture *gTexture = nullptr;
mat4x4 MakeLookAt(vec3 camerapos, vec3 up, vec3 target);
mat4x4 MakeViewMatrix(vec3 forward, vec3 right, vec3 up, vec3 campos);
void init();
void close_program();

struct vert {
  vert() = default;
  vec3 p;
  vec2 tc;
  vert(std::initializer_list<float> vals) {
    if (vals.size() != 5)
      throw new std::invalid_argument("invalid arg");

    auto it = vals.begin();
    p.x = *it++;
    p.y = *it++;
    p.z = *it++;
    tc.x = *it++;
    tc.y = *it;
  }
};
struct triangle {
  vert p[3];
  triangle(std::initializer_list<vert> vals) {
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
      {{0, 1, 0, 0, 0}, {1, 1, 0, 1, 0}, {0, 0, 0, 0, 1}},
      {{1, 0, 0, 1, 1}, {0, 0, 0, 0, 1}, {1, 1, 0, 1, 0}},
      /**/
      /* // BACK FACE */
      {{1, 0, 1, 0, 1}, {1, 1, 1, 0, 0}, {0, 1, 1, 1, 0}},
      {{1, 0, 1, 0, 1}, {0, 1, 1, 1, 0}, {0, 0, 1, 1, 1}},
      // LFFT FACE
      {{0, 1, 1, 0, 0}, {0, 1, 0, 1, 0}, {0, 0, 1, 0, 1}},
      {{0, 1, 0, 1, 0}, {0, 0, 0, 1, 1}, {0, 0, 1, 0, 1}},
      /**/
      /* // RIGHT FACE */
      {{1, 1, 0, 0, 0}, {1, 1, 1, 1, 0}, {1, 0, 1, 1, 1}},
      {{1, 0, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 1, 0, 0, 0}},

      // TOP
      {{0, 1, 0, 0, 1}, {0, 1, 1, 0, 0}, {1, 1, 1, 1, 0}},
      {{1, 1, 1, 1, 0}, {1, 1, 0, 1, 1}, {0, 1, 0, 0, 1}},

      // BOTTOM
      {{0, 0, 0, 0, 0}, {1, 0, 1, 1, 1}, {0, 0, 1, 0, 1}},
      {{1, 0, 1, 1, 1}, {0, 0, 0, 0, 0}, {1, 0, 0, 1, 0}}

      /**/
  };
  const char *names[] = {"FRONT", "BACK", "LEFT", "RIGHT", "TOP", "Bottom"};
  //
  //
  float i = 0;
  for (auto tri : cube.tris) {
    vec3 normal = Cross(tri.p[1].p - tri.p[0].p, tri.p[2].p - tri.p[0].p);
    normal.normalize();
    printf("%s face normal: (%f, %f, %f)\n", names[(int)i], normal.x, normal.y,
           normal.z);
    i += 0.5;
  }
  float fov = M_PI / 2;
  float aspectRatio = static_cast<float>(WINDOW_HEIGHT) / WINDOW_HEIGHT;
  float zFar = 100;
  float zNear = 0.01;
  float fBank = 0.0;
  float fYaw = 0.0;
  vec3 camera = {0, 0, 0};
  float xOffset = 0;
  float yOffset = 0;
  float zOffset = 3;
  mat4x4 clipMatrix = ClipPrespective(aspectRatio, fov, zNear, zFar);

  std::vector<SDL_Vertex> projectedMesh;

  bool quit = false;
  SDL_Event e;
  while (!quit) {
    projectedMesh.clear();
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
      if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_w: {

          camera.z += 0.1;
          break;
        }
        case SDLK_s: {
          camera.z -= 0.1;
          break;
        }
        case SDLK_a: {
          camera.x -= 0.1;
          break;
        }
        case SDLK_d: {
          camera.x += 0.1;
          break;
        }
        case SDLK_UP: {
          camera.y += 0.1;
          break;
        }
        case SDLK_DOWN: {
          camera.y -= 0.1;
          break;
        }
        case SDLK_e: {
          /* fYaw += 0.02; */
          break;
        }
        case SDLK_r: {
          /* fYaw -= 0.02; */
          break;
        }
        }
      }
    }
    fYaw += 0.02;
    fBank += 0.02;
    for (auto tri : cube.tris) {

      // some goofy rotation
      mat4x4 ry = RotateAxis({0, 1, 0}, fYaw);
      mat4x4 rz = RotateAxis({0, 0, 1}, fBank);
      mat4x4 sc = ScaleUniform(0.5);
      ;
      for (int i = 0; i < 3; i++) {

        tri.p[i].p = tri.p[i].p * sc * ry * rz;
      }
      for (int i = 0; i < 3; i++) {

        tri.p[i].p.z += zOffset;
        tri.p[i].p.y += yOffset;
        tri.p[i].p.x += xOffset;
      }
      vec3 normal = Cross(tri.p[1].p - tri.p[0].p, tri.p[2].p - tri.p[0].p);
      normal.normalize();

      vec3 lookvec = {0, 0, 1};

      float theta =
          (lookvec * normal) / (lookvec.magnitude() * normal.magnitude());

      if (theta < 0) {

        for (int i = 0; i < 3; i++) {

          vec3 target = {0, 0, 1};
          /* target = target * RotateY(fYaw); */
          target += camera;
          vec3 up = {0, 1, 0};
          mat4x4 lookAt = MakeLookAt(camera, up, target);
          vert v = tri.p[i];
          v.p = v.p * lookAt;

          // our premitive camera
          vec4 projectVec = vec4{v.p, 1} * clipMatrix;
          // perspective divide
          projectVec.x /= projectVec.w;
          projectVec.y /= projectVec.w;
          projectVec.z /= projectVec.w;

          SDL_Vertex ver;
          ver.position.x = (projectVec.x + 1) * 0.5 * WINDOW_WIDTH;
          ver.position.y =
              WINDOW_HEIGHT - (projectVec.y + 1) * 0.5 * WINDOW_HEIGHT;

          ver.color = {255, 255, 255, 255};
          ver.tex_coord.x = v.tc.x;
          ver.tex_coord.y = v.tc.y;
          projectedMesh.push_back(ver);
        }
      }
      // projecting to screen space
    }
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xff);

    SDL_RenderGeometry(gRenderer, gTexture, projectedMesh.data(),
                       projectedMesh.size(), NULL, 0);
    SDL_RenderPresent(gRenderer);
    SDL_RenderClear(gRenderer);
  }
  close_program();
  return 0;
}
void init() {
  if (SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "Error Occured: %s\n", SDL_GetError());
    exit(1);
  }
  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
    fprintf(stderr, "Error Occured: %s\n", IMG_GetError());
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
  gTexture = IMG_LoadTexture(gRenderer, "./assets/textures/brick.png");
  if (!gTexture) {

    fprintf(stderr, "Error Occured: %s\n", IMG_GetError());
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
mat4x4 MakeViewMatrix(vec3 forward, vec3 right, vec3 up, vec3 campos) {

  float tx = campos * right;
  float ty = campos * up;
  float tz = campos * forward;
  return {right.x,   right.y,   right.z,   0, up.x, up.y, up.z, 0,
          forward.x, forward.y, forward.z, 0, -tx,  -ty,  -tz,  1};
}

mat4x4 MakeLookAt(vec3 camerapos, vec3 up, vec3 target) {
  vec3 forward = target - camerapos;
  forward.normalize();

  vec3 newup = up - (forward * (forward * up));
  newup.normalize();

  vec3 right = Cross(newup, forward);
  mat4x4 m = {right.x,     right.y,     right.z,     0,
              newup.x,     newup.y,     newup.z,     0,
              forward.x,   forward.y,   forward.z,   0,
              camerapos.x, camerapos.y, camerapos.z, 1};
  m.inverse();
  return m;
}
