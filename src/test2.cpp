#include "../include/FireEngine/Camera.hpp"
#include "../include/FireEngine/Light.hpp"
#include "../include/FireEngine/premitives.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <Timer.hpp>
#include <Utils/FPSTimer.hpp>
#include <Utils/Texture.hpp>
#include <algorithm>
#include <list>
#include <uml/mat4x4.h>
#include <uml/transform.h>
#include <uml/vec2.h>
#include <uml/vec3.h>
#include <uml/vec4.h>
#include <vector>

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;
constexpr bool RENDER_SKELETON = false;
SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;
TTF_Font *gFont = nullptr;
SDL_Texture *gTexture = nullptr;
void init();
void close_program();
float DISABLE_BACKFACE_CULLING = false;
using namespace FireEngine;
int main(int argc, char **argv) {
  init();

  printf("%s program running with %d args\n", argv[0], argc);
  // FPS TIMER
  SDL_Color col = {123, 22, 88};

  ETexture *fpsTexture = new ETexture(gRenderer, "test", gFont, col);
  FPSTimer *fpsTimer = new FPSTimer(fpsTexture, gFont, col);

  // Object 3D (Mesh)

  Object3D cube;
  cube.LoadFromObj("./assets/objects/teapot.obj");
  DirectionalLight direLight(vec3(0, -1, 0), vec3(1.0, 0.5, 0.5), 1);
  cube.setRotation(M_PI / 3, 0, 0);
  cube.setPosition({0, 0, 2});
  constexpr float zNear = 0.1;
  constexpr float zFar = 9;
  constexpr float t = 80.0 * (3.1415 / 180);
  constexpr float fFov = t;

  // Camera
  float fYaw = 0.0f;
  float fTargetYaw = 0.0f;
  float fAspectRatio = static_cast<float>(WINDOW_HEIGHT) / WINDOW_WIDTH;
  Camera perCamera = PerspectiveCamera(fAspectRatio, fFov, zNear, zFar);
  mat4x4 clipMatrix = perCamera.getProjectionTransform();

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
          perCamera.Translate(0, 0.2, 0);
          break;
        }
        case SDLK_DOWN: {
          perCamera.Translate(0, -0.2, 0);
          break;
        }
        case SDLK_w: {
          perCamera.Translate(0, 0, 0.2);
          break;
        }
        case SDLK_s: {
          perCamera.Translate(0, 0, -0.2);
          break;
        }
        case SDLK_a: {
          perCamera.Translate(-0.2, 0, 0);
          break;
        }
        case SDLK_d: {
          perCamera.Translate(0.2, 0, 0);
          break;
        }
        case SDLK_e: {
          fTargetYaw -= 0.2;
          perCamera.setOrientation(fTargetYaw, 0, 0);
          break;
        }
        case SDLK_r: {

          fTargetYaw += 0.2;
          perCamera.setOrientation(fTargetYaw, 0, 0);
          break;
        }
        }
      }
    }
    // object transform

    // camera
    std::vector<triangle> projectedMesh;
    // Rendering operation
    for (auto tri : cube.triangles) {

      // transforming each vertex of the triangle

      tri = tri * cube.transform;
      vec3 normal = tri.comptueNormal();
      // backface culling
      //  TODO: fix this
      float theta = normal * (tri.verticies[0].position - perCamera.position);

      if (theta < 0) {

        tri = tri * perCamera.getViewTransform();
        // znear clipping
        std::vector<triangle> clippedTriangles =
            PlaneClipTriangle({0, 0, 1}, {0, 0, zNear}, tri);
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
          projectedTriangle.color = clippedTri.color;

          std::list<triangle> listTriangles;
          listTriangles.push_back(projectedTriangle);
          uint8_t newTrianglesCount = 1;

          for (uint8_t p = 0; p < 4; p++) {
            while (newTrianglesCount > 0) {
              triangle test = listTriangles.front();
              std::vector<triangle> newTriangles;
              listTriangles.pop_front();
              newTrianglesCount--;

              switch (p) {
              case 0: {
                // left screen edge
                newTriangles =
                    PlaneClipTriangle({1.0, 0, 0}, {-1.0, 0, 0}, test);
                break;
              }
              case 1: {
                // top screen edge

                newTriangles = PlaneClipTriangle({0, -1, 0}, {0, 1.0, 0}, test);
                break;
              }
              case 2: {
                // right screen edge
                newTriangles = PlaneClipTriangle({-1, 0, 0}, {1.0, 0, 0}, test);
                break;
              }
              case 3: {
                // bottom screen edge
                newTriangles = PlaneClipTriangle({0, 1, 0}, {0, -1.0, 0}, test);
                break;
              }
              }

              for (auto &tr : newTriangles) {
                listTriangles.push_back(tr);
              }
            }
            newTrianglesCount = listTriangles.size();
          }

          for (auto &tr : listTriangles) {
            tr.normal = normal;
            projectedMesh.push_back(tr);
          }
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
    for (auto &tri : projectedMesh) {

      for (int j = 0; j < 3; j++) {
        const vertex &vert = tri.verticies[j];
        SDL_Vertex finalvert;

        // calculating basic lambertain lighting using per-triangle normals,
        // this will obviously result in a very janky flat shading, but this is
        // part of the learnign process
        float ln =
            -direLight.getDirection() * tri.normal * direLight.getIntensity();
        finalvert.position.x = (vert.position.x + 1) * 0.5 * WINDOW_WIDTH;
        finalvert.position.y =
            WINDOW_HEIGHT - (vert.position.y + 1) * 0.5 * WINDOW_HEIGHT;
        FE_CLAMP(ln, 0, 1);
        vec3 color = direLight.getColor() & tri.color * ln * UINT8_MAX;
        finalvert.color = {static_cast<uint8_t>(color.x),
                           static_cast<uint8_t>(color.y),
                           static_cast<uint8_t>(color.z), 0xff};

        finalvert.tex_coord = {0};
        vertexToRender[k++] = finalvert;
        tri.verticies[j].position.x = finalvert.position.x;
        tri.verticies[j].position.y = finalvert.position.y;
      }
      //    tri.verticies
      if (RENDER_SKELETON) {
        SDL_SetRenderDrawColor(gRenderer, 0xff * tri.color.x,
                               0xff * tri.color.y, 0xff * tri.color.z, 0xFF);

        SDL_RenderDrawLineF(
            gRenderer, tri.verticies[0].position.x, tri.verticies[0].position.y,
            tri.verticies[1].position.x, tri.verticies[1].position.y);
        SDL_RenderDrawLineF(
            gRenderer, tri.verticies[0].position.x, tri.verticies[0].position.y,
            tri.verticies[2].position.x, tri.verticies[2].position.y);
        SDL_RenderDrawLineF(
            gRenderer, tri.verticies[2].position.x, tri.verticies[2].position.y,
            tri.verticies[1].position.x, tri.verticies[1].position.y);
      }
    }

    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

    if (!RENDER_SKELETON) {
      SDL_RenderGeometry(gRenderer, nullptr, vertexToRender.data(),
                         vertexToRender.size(), nullptr, 0);
    }
    fpsTimer->displayFPS();
    SDL_RenderPresent(gRenderer);
    SDL_RenderClear(gRenderer);
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
