#ifndef _FIRE_ENGINE_WORLD_HPP
#define _FIRE_ENGINE_WORLD_HPP
#include "Light.hpp"
#include "premitives.hpp"
#include <memory>
namespace FireEngine {
struct World {

  std::vector<std::shared_ptr<Object3D>> ObjectList;
  std::vector<std::shared_ptr<Light>> Lights;

  inline void AddObject(std::shared_ptr<Object3D> objptr) {
    ObjectList.push_back(objptr);
  }
  inline void AddLight(std::shared_ptr<Light> lightptr) {
    Lights.push_back(lightptr);
  }
};
} // namespace FireEngine
#endif
