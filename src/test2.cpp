#include "../include/premitives.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <Timer.hpp>
#include <Utils/FPSTimer.hpp>
#include <Utils/Texture.hpp>
#include <algorithm>

#include <stdint.h>
#include <uml/transform.h>
#include <uml/vec2.h>
#include <uml/vec3.h>
#include <uml/vec4.h>
#include <vector>
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;
TTF_Font *gFont = nullptr;
SDL_Texture *gTexture = nullptr;
void init();
void close_program();
float DISABLE_BACKFACE_CULLING = false;

int main(int argc, char **argv) {
  init();

  printf("%s program running with %d args\n", argv[0], argc);
  SDL_Color col = {123, 22, 88};

  ETexture *fpsTexture = new ETexture(gRenderer, "test", gFont, col);
  FPSTimer *fpsTimer = new FPSTimer(fpsTexture, gFont, col);

  mesh cube;
  cube.LoadObj("./assets/objects/teapot.obj");
  float zNear = 0.1;
  float zFar = 9;
  float t = 80.0 * (3.1415 / 180);
  float fFov = t;

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
  fpsTimer->resetTimer();
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
    mat4x4 ry = RotateY(fYaw);
    mat4x4 rx = RotateX(fYaw);
    mat4x4 rz = RotateZ(fBank);
    mat4x4 sc = ScaleUniform(1);
    vec3 target = {0, 0, 1};
    target = target * RotateY(fTargetYaw);
    lookDir = target - cameraPosition;
    lookDir.normalize();
    mat4x4 viewMatrix =
        LookAt(cameraPosition, target + cameraPosition, {0, 1, 0});

    std::vector<triangle> projectedMesh;
    for (auto tri : cube.triangles) {

      // very basic world transform

      // transforming each vertex of the triangle
      for (size_t i = 0; i < 3; i++) {

        vec3 pos = tri.verticies[i].position;
        tri.verticies[i].position = pos * rz * rx * ry;
        tri.verticies[i].position.z += 11;

        // view transform goes here
        //
        //
      }

      // frustum culling
      //
      /* std::vector<triangle> clippedTriangles = */
      /* PlaneClipTriangle(lookDir, {0, 0, 0.5}, tri); */

      vec3 q0 = tri.verticies[1].position - tri.verticies[0].position;
      vec3 q1 = tri.verticies[2].position - tri.verticies[0].position;
      vec3 normal = Cross(q0, q1);
      normal.normalize();

      float theta = normal * lookDir;

      if (theta < 0) {
        for (size_t i = 0; i < 3; i++) {

          // view transform
          //
          vec3 pos = tri.verticies[i].position;
          tri.verticies[i].position = pos * viewMatrix;
        }

        std::vector<triangle> clippedTriangles =
            PlaneClipTriangle(lookDir, {0, 0, 0.5}, tri);
        for (auto &clippedTri : clippedTriangles) {
          triangle projectedTriangle;
          for (size_t i = 0; i < 3; i++) {
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
          }
          vec3 q0 = clippedTri.verticies[1].position -
                    clippedTri.verticies[0].position;
          vec3 q1 = clippedTri.verticies[2].position -
                    clippedTri.verticies[0].position;
          vec3 normal = Cross(q0, q1);
          normal.normalize();

          projectedTriangle.normal = normal;
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

    size_t projectedMeshTriCount = projectedMesh.size();
    vertexToRender.resize(projectedMeshTriCount * 3);
    int k = 0;
    for (size_t i = 0; i < projectedMeshTriCount; i++) {
      triangle tri = projectedMesh[i];

      for (int j = 0; j < 3; j++) {
        vertex vert = tri.verticies[j];
        SDL_Vertex finalvert;

        // calculating basic lambertain lighting using per-triangle normals,
        // this will obviously result in a very janky flat shading, but this is
        // part of the learnign process
        float ln = -lookDir * tri.normal;
        finalvert.position.x = (vert.position.x + 1) * 0.5 * WINDOW_WIDTH;
        finalvert.position.y =
            WINDOW_HEIGHT - (vert.position.y + 1) * 0.5 * WINDOW_HEIGHT;
        uint8_t color = ln * 255.0;
        finalvert.color = {color, color, color, 0xff};
        finalvert.tex_coord = {0};
        vertexToRender[k++] = finalvert;
      }
    }

    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(gRenderer);
    SDL_RenderGeometry(gRenderer, nullptr, vertexToRender.data(),
                       vertexToRender.size(), nullptr, 0);
    fpsTimer->displayFPS();
    SDL_RenderPresent(gRenderer);
    /* fYaw += 0.02; */
    /* quit = true; */
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

  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
  if (!gRenderer) {
    fprintf(stderr, "Error Occured: %s\n", SDL_GetError());
    exit(1);
  }
  if (TTF_Init()) {
    fprintf(stderr, "Error occured %s", SDL_GetError());
    exit(1);
  }
  gFont = TTF_OpenFont("./assets/font/Pixeled.ttf", 20);
  if (!gFont) {
    fprintf(stderr, "Error occured %s", SDL_GetError());
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
