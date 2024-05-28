#ifndef _FIRE_ENGINE_LIGHT_HPP
#define _FIRE_ENGINE_LIGHT_HPP

#include "premitives.hpp"
#include <uml/vec3.h>

namespace FireEngine {

// TODO: a better way to represent lights and more types of lights
enum class LIGHT_TYPE { EMISSIVE, DIRECTIONAL, SPOT, POINT, UNDEFINED };
struct Light {

  vec3 color;
  float intensity;
  LIGHT_TYPE type = LIGHT_TYPE::UNDEFINED;
  Light(const vec3 &_col, float _intensity)
      : color{_col}, intensity{_intensity} {}

  virtual vec3 calculateLambertian(const vec3 &fragPos,
                                   const vec3 &normal) const = 0;
};
struct DirectionalLight : public Light {

  vec3 direction;

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
  vec3 calculateLambertian(const vec3 &fragPos,
                           const vec3 &normal) const override {

    (void)(fragPos);
    vec3 _n = color;
    float diff = -normal * direction * intensity;
    vec3 ln = _n * FE_MAX(0, diff);

    return ln;
  }

  inline float getIntensity(void) { return intensity; }
  inline vec3 getDirection(void) { return direction; }
  inline vec3 getColor(void) { return color; }
};
} // namespace FireEngine
#endif
