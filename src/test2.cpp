#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <algorithm>
#include <stdint.h>
#include <uml/transform.h>

#include "../include/premitives.hpp"
#include <uml/vec2.h>
#include <uml/vec3.h>
#include <uml/vec4.h>
#include <vector>
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;
mat4x4 MakeLookAt(vec3 position, vec3 target, vec3 up);
SDL_Texture *gTexture = nullptr;
void init();
void close_program();
float DISABLE_BACKFACE_CULLING = false;

int main(int argc, char **argv) {
  init();

  printf("%s program running with %d args\n", argv[0], argc);
  mesh cube;
  cube.triangles = {{{0, 1, 0}, {1, 1, 0}, {0, 0, 0}},
                    {{1, 0, 0}, {0, 0, 0}, {1, 1, 0}},

                    /* // BACK */
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

                    // BOTTOM
                    {{0, 0, 0}, {1, 0, 1}, {0, 0, 1}},
                    {{1, 0, 1}, {0, 0, 0}, {1, 0, 0}}

  };
  printf("%zu\n", cube.triangles.size());
  float zNear = 1;
  float zFar = 70;
  float fFov = M_PI / 2;

  float fYaw = 0.0f;
  float fTargetYaw = 0.0f;
  float fBank = 0.0f;
  float fPitch = 0.0f;
  float fAspectRatio = static_cast<float>(WINDOW_HEIGHT) / WINDOW_WIDTH;

  mat4x4 clipMatrix = ClipPrespective(fAspectRatio, fFov, zNear, zFar);

  vec3 lookDir = {0, 0, 1}; // we are looking toward z direction
  vec3 lightDir = {0, 0, 1};
  vec3 cameraPosition = {0, 0, 0};
  bool quit = false;
  SDL_Event e;
  while (!quit) {

    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
      if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_UP: {
          cameraPosition.y += 0.2;
          break;
        }
        case SDLK_DOWN: {
          cameraPosition.y -= 0.2;
          break;
        }
        case SDLK_w: {
          cameraPosition.z += 0.2;
          break;
        }
        case SDLK_s: {
          cameraPosition.z -= 0.2;
          break;
        }
        case SDLK_a: {
          cameraPosition.x -= 0.2;
          break;
        }
        case SDLK_d: {
          cameraPosition.x += 0.2;
          break;
        }
        case SDLK_e: {
          fTargetYaw -= 0.02;
          break;
        }
        case SDLK_r: {

          fTargetYaw += 0.02;
          break;
        }
        }
      }
    }

    std::vector<triangle> projectedMesh;
    for (auto tri : cube.triangles) {

      // very basic world transform

      mat4x4 translate = TranslateXYZ(-0.7, 0, 5);
      mat4x4 ry = RotateY(fYaw);
      mat4x4 rx = RotateX(fPitch);
      mat4x4 rz = RotateZ(fBank);

      // transforming each vertex of the triangle
      for (size_t i = 0; i < 3; i++) {
        vec3 pos = tri.verticies[i].position;
        tri.verticies[i].position = pos * rz * rx * ry * translate;
      }

      // view transform goes here
      //
      //
      for (size_t i = 0; i < 3; i++) {

        vec3 target = {0, 0, 1};
        target = target * RotateY(fTargetYaw);
        mat4x4 viewMatrix =
            LookAt(cameraPosition, target + cameraPosition, {0, 1, 0});
        vec3 pos = tri.verticies[i].position;
        tri.verticies[i].position = pos * viewMatrix;
        target = lookDir;
      }

      // frustum culling
      //
      std::vector<triangle> clippedTriangles =
          PlaneClipTriangle(lookDir, {0, 0, 0.5}, tri);

      for (auto clippedTri : clippedTriangles) {
        vec3 q0 =
            clippedTri.verticies[1].position - clippedTri.verticies[0].position;
        vec3 q1 =
            clippedTri.verticies[2].position - clippedTri.verticies[0].position;
        vec3 normal = Cross(q0, q1);

        float theta = normal * lookDir;

        triangle projectedTriangle;
        if (theta < 0 || DISABLE_BACKFACE_CULLING) {
          for (size_t i = 0; i < 3; i++) {

            // cliping time
            //
            //
            vec4 projectedVec = vec4{clippedTri.verticies[i].position, 1};
            projectedVec = projectedVec * clipMatrix;
            // prespective divide
            //
            projectedVec.x /= projectedVec.w;
            projectedVec.y /= projectedVec.w;
            projectedVec.z /= projectedVec.w;

            vertex projectedVertex = {projectedVec.x, projectedVec.y,
                                      projectedVec.z};
            projectedTriangle.verticies[i] = projectedVertex;
            projectedTriangle.normal = normal;
          }
          projectedMesh.push_back(projectedTriangle);
        }
      }
    }

    // painters algorithm
    std::sort(projectedMesh.begin(), projectedMesh.end(),
              [](const triangle &a, const triangle &b) {
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

    std::vector<SDL_Vertex> vertexToRender;

    for (const auto &tri : projectedMesh) {
      for (size_t i = 0; i < 3; i++) {
        vertex vert = tri.verticies[i];
        SDL_Vertex finalvert;

        // calculating basic lambertain lighting using per-triangle normals,
        // this will obviously result in a very janky flat shading, but this is
        // part of the learnign process

        auto max = [](const float a, const float b) {
          if (a > b)
            return a;
          return b;
        };
        float ln = -lookDir * tri.normal;
        finalvert.position.x = (vert.position.x + 1) * 0.5 * WINDOW_WIDTH;
        finalvert.position.y =
            WINDOW_HEIGHT - (vert.position.y + 1) * 0.5 * WINDOW_HEIGHT;
        uint8_t color = max(ln, 0) * 0xff;
        finalvert.color = {color, color, color, 0xff};
        vertexToRender.push_back(finalvert);
      }
    }
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(gRenderer);
    SDL_RenderGeometry(gRenderer, NULL, vertexToRender.data(),
                       vertexToRender.size(), NULL, 0);
    SDL_RenderPresent(gRenderer);
    fYaw += 0.02;
    /* fBank += 0.12; */
    /* fPitch += 0.009; */
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
  /* gTexture = IMG_LoadTexture(gRenderer, "./assets/textures/brick.png"); */
  /* if (!gTexture) { */
  /**/
  /*   fprintf(stderr, "Error Occured: %s\n", IMG_GetError()); */
  /*   exit(1); */
  /* } */
}
void close_program() {
  SDL_DestroyWindow(gWindow);
  gWindow = nullptr;
  SDL_DestroyRenderer(gRenderer);
  gRenderer = nullptr;

  SDL_Quit();

  printf("Closed Gracefully\n");
}
