#include "../include/FireEngine/Camera.hpp"
#include "../include/FireEngine/Light.hpp"
#include "../include/FireEngine/Renderer.hpp"
#include "../include/FireEngine/premitives.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <Timer.hpp>
#include <Utils/FPSTimer.hpp>
#include <Utils/Texture.hpp>
#include <algorithm>
#include <list>
#include <memory>
#include <uml/mat4x4.h>
#include <uml/transform.h>
#include <uml/vec2.h>
#include <uml/vec3.h>
#include <uml/vec4.h>
#include <vector>

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
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
  Object3D realCube;
  Object3D ship;
  Object3D axis;
  axis.LoadFromObj("./assets/objects/axis.obj");
  axis.setScale(0.2, 0.2, 0.2);
  axis.setRotation(M_PI, 0, 0);
  axis.setPosition({-3, 10, 8});
  ship.setPosition({-8, 10, 5});
  ship.LoadFromObj("./assets/objects/VideoShip.obj");
  ship.setScale(0.2, 0.2, 0.2);
  ship.setRotation(M_PI, 0, 0);
  ship.setPosition({0, 10, 5});
  realCube.LoadFromObj("./assets/objects/mountains.obj");

  cube.LoadFromObj("./assets/objects/Suzanne.obj");
  cube.setRotation(-M_PI / 2, 0, 0);
  cube.setPosition({0, 10, 0});
  cube.setScale(0.2, 0.2, 0.2);

  DirectionalLight direLight(vec3(0, 0, 1), vec3(1.0, 0.5, 0.5), 1);
  DirectionalLight direLight2(vec3(0, -1, 0), vec3(0.0, 0.6, 0.5), 1);
  DirectionalLight direLight3(vec3(1, -1, 0), vec3(0.4, 0.0, 0.5), 1);

  cube.setRotation(M_PI / 3, 0, 0);
  cube.setPosition({0, 10, 2});
  constexpr float zNear = 0.1;
  constexpr float zFar = 9;
  constexpr float t = 80.0 * (3.1415 / 180);
  constexpr float fFov = t;

  // Camera
  float fTargetYaw = 0.0f;
  float fAspectRatio = static_cast<float>(WINDOW_HEIGHT) / WINDOW_WIDTH;
  Camera perCamera = PerspectiveCamera(fAspectRatio, fFov, zNear, zFar);
  World gamewrld;
  gamewrld.AddObject(std::make_shared<Object3D>(cube));
  gamewrld.AddObject(std::make_shared<Object3D>(ship));
  gamewrld.AddObject(std::make_shared<Object3D>(realCube));
  gamewrld.AddObject(std::make_shared<Object3D>(axis));
  gamewrld.AddLight(std::make_shared<DirectionalLight>(direLight));
  gamewrld.AddLight(std::make_shared<DirectionalLight>(direLight2));
  gamewrld.AddLight(std::make_shared<DirectionalLight>(direLight3));
  Renderer renderer(gRenderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});
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
    renderer.Render(gamewrld, perCamera);
    fpsTimer->displayFPS();
    SDL_RenderPresent(gRenderer);
    SDL_SetRenderDrawColor(gRenderer, 145, 224, 255, 0xFF);
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
