#include "../include/FireEngine/Camera.hpp"
#include "../include/FireEngine/Light.hpp"
#include "../include/FireEngine/Renderer.hpp"
#include "../include/FireEngine/premitives.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <Timer.hpp>
#include <Utils/FPSTimer.hpp>
#include <Utils/Texture.hpp>
#include <algorithm>
#include <list>
#include <memory>
#include <stdio.h>
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
  Uint32 previousTime = SDL_GetTicks();

  float deltaTime = 0.0f;

  printf("%s program running with %d args\n", argv[0], argc);
  // FPS TIMER
  SDL_Color col = {123, 22, 88};

  ETexture *fpsTexture = new ETexture(gRenderer, "test", gFont, col);
  FPSTimer *fpsTimer = new FPSTimer(fpsTexture, gFont, col);

  // Object 3D (Mesh)

  Object3D cube;
  SDL_Texture *tex = IMG_LoadTexture(gRenderer, "./assets/textures/level0.bmp");
  cube.LoadFromObj("./assets/objects/level0.obj");

  cube.SetImageTexture(tex);
  DirectionalLight direLight(vec3(0, 0, 1), vec3(1.0, 1.0, 1.0), 1);
  DirectionalLight direLight2(vec3(0, -1, 0), vec3(1, 1, 1), 1);
  DirectionalLight direLight3(vec3(1, -1, 0), vec3(1, 1, 1), 1);
  DirectionalLight direLight4(vec3(1, 1, 0), vec3(1, 1, 1), 1);
  constexpr float zNear = 0.1;
  constexpr float zFar = 80;
  constexpr float t = 80.0 * (3.1415 / 180);
  constexpr float fFov = t;

  // Camera
  float fTargetYaw = 0.0f;
  float fTargetPitch = 0.0f;
  float fAspectRatio = static_cast<float>(WINDOW_HEIGHT) / WINDOW_WIDTH;
  Camera perCamera = PerspectiveCamera(fAspectRatio, fFov, zNear, zFar);
  perCamera.setPosition({0, 0, -30});
  World gamewrld;
  gamewrld.AddObject(std::make_shared<Object3D>(cube));
  gamewrld.AddLight(std::make_shared<DirectionalLight>(direLight));
  gamewrld.AddLight(std::make_shared<DirectionalLight>(direLight2));
  gamewrld.AddLight(std::make_shared<DirectionalLight>(direLight3));
  gamewrld.AddLight(std::make_shared<DirectionalLight>(direLight4));
  Renderer renderer(gRenderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});

  FireEngine_RendererFlags flags = static_cast<FireEngine_RendererFlags>(
      FireEngine_RendererFlags_EnableTextures);

  renderer.flags = flags;

  bool quit = false;
  SDL_Event e;
  fpsTimer->resetTimer();
  while (!quit) {
    Uint32 currentTime = SDL_GetTicks();
    deltaTime = (currentTime - previousTime) / 1000.0f; // Convert to seconds
    previousTime = currentTime;
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
          perCamera.setOrientation(fTargetYaw, fTargetPitch, 0);
          break;
        }
        case SDLK_r: {

          fTargetYaw += 0.2;

          perCamera.setOrientation(fTargetYaw, fTargetPitch, 0);
          break;
        }
        case SDLK_f: {

          fTargetPitch -= 0.2;

          perCamera.setOrientation(fTargetYaw, fTargetPitch, 0);
          break;
        }
        case SDLK_v: {

          fTargetPitch += 0.2;

          perCamera.setOrientation(fTargetYaw, fTargetPitch, 0);
          break;
        }
        }
      }
    }
    // object transform

    // camera
    renderer.Render(gamewrld, perCamera, deltaTime);
    fpsTimer->displayFPS();
    SDL_RenderPresent(gRenderer);
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xff);
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

  gWindow = SDL_CreateWindow(
      "Burn!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
      WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);

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
