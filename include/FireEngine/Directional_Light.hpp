#ifndef _FIRE_ENGINE_DIRECTIONAL_LIGHT_HPP
#define _FIRE_ENGINE_DIRECTIONAL_LIGHT_HPP

#include "premitives.hpp"

namespace FireEngine {
struct DirectionalLight {

  vec3 direction;
  vec3 color;
  float intensity;

  DirectionalLight() = default;
  DirectionalLight(vec3 _direction, vec3 _color, float _intensity)
      : direction{_direction}, color{_color}, intensity{_intensity} {}
  inline void setDirection(const vec3 &_direction) { direction = _direction; }
  inline void setColor(const vec3 &_color) { color = _color; }
  inline void setIntensity(float _intensity) {
    FE_CLAMP(_intensity, 0, 1);
    intensity = _intensity;
  }

  inline float getIntensity(void) { return intensity; }
  inline vec3 getDirection(void) { return direction; }
  inline vec3 getColor(void) { return color; }
};
} // namespace FireEngine
#endif
