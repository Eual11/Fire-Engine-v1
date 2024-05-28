#ifndef _FIRE_ENGINE_DIRECTIONAL_LIGHT_HPP
#define _FIRE_ENGINE_DIRECTIONAL_LIGHT_HPP

#include "premitives.hpp"

namespace FireEngine {

// TODO: a better way to represent lights and more types of lights
enum class LIGHT_TYPE { EMISSIVE, DIRECTIONAL, SPOT, POINT, UNDEFINED };
struct Light {

  vec3 color;
  float intensity;
  LIGHT_TYPE type = LIGHT_TYPE::UNDEFINED;
  Light(const vec3 &_col, float _intensity)
      : color{_col}, intensity{_intensity} {}
};
struct DirectionalLight : public Light {

  vec3 direction;
  vec3 color;
  float intensity;

  DirectionalLight(vec3 _direction, vec3 _color, float _intensity)
      : Light{_color, _intensity}, direction{_direction} {
    type = LIGHT_TYPE::DIRECTIONAL;
  }
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
