#include "../include/FireEngine/Camera.hpp"
#include "../include/FireEngine/Light.hpp"
#include "../include/FireEngine/Renderer.hpp"
#include "../include/FireEngine/World.hpp"
#include "../include/FireEngine/premitives.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstdio>
#include <memory>
#include <uml/mat4x4.h>
#include <uml/transform.h>
#include <uml/vec2.h>
#include <uml/vec3.h>
#include <uml/vec4.h>

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;

void init();
void close_program();

int main(int argc, char **argv) {

  init();

  FireEngine::Object3D obj;
  FireEngine::Object3D obj1;
  FireEngine::Object3D obj2;
  obj1.LoadFromObj("./assets/objects/teapot.obj");
  obj.LoadFromObj("./assets/objects/Suzanne.obj");
  obj2.LoadFromObj("./assets/objects/max-planck.obj");
  obj.setPosition({0, 0, 0});
  obj1.setPosition({6, 0, 4});
  obj1.setScale(0.5, 0.5, 0.5);

  obj2.setPosition({-25, 0, 40});
  obj2.setScale(0.06, 0.06, 0.06);

  float fTargetYaw = 0.0f;
  float zNear = 0.1;
  constexpr float zFar = 100;
  constexpr float t = 80.0 * (3.1415 / 180);
  constexpr float fFov = t;

  float fAspectRatio = static_cast<float>(WINDOW_HEIGHT) / WINDOW_WIDTH;
  FireEngine::Camera perCamera =
      FireEngine::PerspectiveCamera(fAspectRatio, fFov, zNear, zFar);
  FireEngine::DirectionalLight dir(vec3(0, 0, 1), vec3(1.0f, 1.0f, 1.0f), 1.0f);

  perCamera.setPosition({0, 0, -4});
  FireEngine::Renderer renderer(gRenderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});
  FireEngine::World world;
  world.AddObject(std::make_shared<FireEngine::Object3D>(obj));
  world.AddObject(std::make_shared<FireEngine::Object3D>(obj1));
  world.AddObject(std::make_shared<FireEngine::Object3D>(obj2));
  world.AddLight(std::make_shared<FireEngine::DirectionalLight>(dir));

  SDL_Log("Program %s running with %d args\n", argv[0], argc);
  SDL_Log("Hello World\n");

  bool quit = false;

  SDL_Event e;
  while (!quit) {

    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
        quit = true;

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
    renderer.Render(world, perCamera, 0);
    SDL_RenderPresent(gRenderer);
    /* SDL_SetRenderDrawColor(gRenderer, 140, 255, 200, 0xFF); */
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(gRenderer);
  }

  close_program();
  return 0;
}

void init() {
  if (SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "Error Occured: %s\n", SDL_GetError());
  }

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {

    fprintf(stderr, "Error Occured: %s\n", IMG_GetError());
  }
  gWindow = SDL_CreateWindow("Firev1", SDL_WINDOWPOS_UNDEFINED,
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
}

void close_program() {
  SDL_DestroyWindow(gWindow);
  gWindow = nullptr;
  SDL_DestroyRenderer(gRenderer);

  SDL_Quit();
  printf("Closed Gracefully\n");
}
