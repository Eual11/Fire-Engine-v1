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
#include <uml/mat4x4.h>
#include <uml/transform.h>
#include <uml/vec2.h>
#include <uml/vec3.h>
#include <uml/vec4.h>
#include <vector>
constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;
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
  constexpr float zNear = 0.1;
  constexpr float zFar = 9;
  constexpr float t = 80.0 * (3.1415 / 180);
  constexpr float fFov = t;

  float fYaw = 0.0f;
  float fTargetYaw = 0.0f;
  float fBank = 0.0f;
  float fPitch = 0.0f;
  float fAspectRatio = static_cast<float>(WINDOW_HEIGHT) / WINDOW_WIDTH;
  mat4x4 clipMatrix = ClipPrespective(fAspectRatio, fFov, zNear, zFar);
  mat4x4 viewMatrix;

  vec3 lookDir = {0, 0, 1}; // we are looking toward z direction
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
          vec3 upVec = {viewMatrix.m[0][1], viewMatrix.m[1][1],
                        viewMatrix.m[2][1]};
          cameraPosition += upVec * 0.2;
          break;
        }
        case SDLK_DOWN: {
          vec3 upVec = {viewMatrix.m[0][1], viewMatrix.m[1][1],
                        viewMatrix.m[2][1]};
          cameraPosition += upVec * -0.2;
          break;
        }
        case SDLK_w: {
          vec3 forwardVec = {viewMatrix.m[0][2], viewMatrix.m[1][2],
                             viewMatrix.m[2][2]};
          cameraPosition += forwardVec * 0.2;
          break;
        }
        case SDLK_s: {
          vec3 forwardVec = {viewMatrix.m[0][2], viewMatrix.m[1][2],
                             viewMatrix.m[2][2]};
          cameraPosition += forwardVec * -0.2;
          break;
        }
        case SDLK_a: {
          vec3 rightVec = {viewMatrix.m[0][0], viewMatrix.m[1][0],
                           viewMatrix.m[2][0]};
          cameraPosition += rightVec * 0.2;
          break;
        }
        case SDLK_d: {
          vec3 rightVec = {viewMatrix.m[0][0], viewMatrix.m[1][0],
                           viewMatrix.m[2][0]};
          cameraPosition += rightVec * -0.2;
          break;
        }
        case SDLK_e: {
          /* printf("(%f, %f, %f)\n", lookDir.x, lookDir.y, lookDir.z); */
          fTargetYaw -= 0.02;
          break;
        }
        case SDLK_r: {

          /* printf("(%f, %f, %f)\n", lookDir.x, lookDir.y, lookDir.z); */
          fTargetYaw += 0.02;
          break;
        }
        }
      }
    }
    mat4x4 ry = RotateY(fYaw);
    mat4x4 rx = RotateX(0.0);
    mat4x4 rz = RotateZ(fBank);
    vec3 target = {0, 0, 1};
    target = target * RotateY(fTargetYaw);
    lookDir = target;
    ;
    lookDir.normalize();
    mat4x4 final_transform = rx * ry * rz;
    viewMatrix = LookAt(cameraPosition, target + cameraPosition, {0, 1, 0});
    std::vector<triangle> projectedMesh;
    for (auto tri : cube.triangles) {

      // transforming each vertex of the triangle
      for (size_t i = 0; i < 3; i++) {

        // very basic world transform
        tri.verticies[i].position = tri.verticies[i].position;
        tri.verticies[i].position.z += 11;
      }

      vec3 q0 = tri.verticies[1].position - tri.verticies[0].position;
      vec3 q1 = tri.verticies[2].position - tri.verticies[0].position;
      vec3 normal = Cross(q0, q1);
      normal.normalize();

      // backface culling
      //  TODO: fix this
      float theta = normal * (tri.verticies[0].position - cameraPosition);

      if (theta < 0) {
        for (size_t i = 0; i < 3; i++) {

          // view transform
          //
          tri.verticies[i].position = tri.verticies[i].position * viewMatrix;
        }
        // znear clipping
        std::vector<triangle> clippedTriangles =
            PlaneClipTriangle({0, 0, 1}, {0, 0, 2.5}, tri);
        triangle projectedTriangle;
        for (auto &clippedTri : clippedTriangles) {
          for (size_t i = 0; i < 3; i++) {
            vec4 projectedVec = vec4{clippedTri.verticies[i].position, 1};
            projectedVec = projectedVec * clipMatrix;
            // prespective divide
            //
            if (projectedVec.w != 0.0f) {
              projectedVec.x /= projectedVec.w;
              projectedVec.y /= projectedVec.w;
              projectedVec.z /= projectedVec.w;
            }

            vertex projectedVertex;
            projectedVertex.position.x = projectedVec.x;

            projectedVertex.position.y = projectedVec.y;
            projectedVertex.position.z = projectedVec.z;
            projectedTriangle.verticies[i] = projectedVertex;
          }

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
    size_t k = 0;
    for (size_t i = 0; i < projectedMeshTriCount; i++) {
      const triangle &tri = projectedMesh[i];

      for (int j = 0; j < 3; j++) {
        const vertex &vert = tri.verticies[j];
        SDL_Vertex finalvert;

        // calculating basic lambertain lighting using per-triangle normals,
        // this will obviously result in a very janky flat shading, but this is
        // part of the learnign process
        float ln = -lookDir * tri.normal;
        finalvert.position.x = (vert.position.x + 1) * 0.5 * WINDOW_WIDTH;
        finalvert.position.y =
            WINDOW_HEIGHT - (vert.position.y + 1) * 0.5 * WINDOW_HEIGHT;
        ln = ln < 0 ? 0 : ln;
        uint8_t color = ln * 255;
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
    fYaw += 0.02;
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
