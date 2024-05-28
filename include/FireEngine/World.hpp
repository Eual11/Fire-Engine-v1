#ifndef _FIRE_ENGINE_WORLD_HPP
#define _FIRE_ENGINE_WORLD_HPP
#include "Light.hpp"
#include "premitives.hpp"
#include <memory>
namespace FireEngine {
struct World {

  std::vector<Object3D> ObjectList;
  std::vector<std::shared_ptr<Light>> Lights;
};
} // namespace FireEngine
#endif
