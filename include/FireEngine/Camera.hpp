#ifndef _FIRE_ENGINE_CAMERA_HPP
#define _FIRE_ENGINE_CAMERA_HPP

#include "premitives.hpp"
#include <uml/transform.h>

namespace FireEngine {
// NOTE: very inefficent, i will leave it for now
struct Camera {
  vec3 position;
  vec3 forward;
  vec3 up;
  vec3 right;
  float aspectRatio;
  float fov;
  float zNear;
  float zFar;
  Camera() {
    forward = {0, 0, 1};
    up = {0, 1, 0};
    right = {1, 0, 0};
    position = {0, 0, 0};
    updateViewTransform();
  }
  mat4x4 viewTransform;
  mat4x4 projectionTransform;
  Camera(float _aspectRatio, float _fov, float _zNear, float _zFar)
      : aspectRatio(_aspectRatio), fov(_fov), zNear(_zNear), zFar(_zFar) {
    forward = {0, 0, 1};
    up = {0, 1, 0};
    right = {1, 0, 0};
    position = {0, 0, 0};
    updateViewTransform();
  }

  inline mat4x4 getViewTransform() { return viewTransform; }
  inline mat4x4 getProjectionTransform() { return projectionTransform; }

  inline void setViewTransform(const mat4x4 &m) { viewTransform = m; }
  inline void setProjectionTransform(const mat4x4 &m) {
    projectionTransform = m;
  }
  void lookAt(vec3 &Target) {
    viewTransform = LookAt(position, Target + position, up);
  }
  inline void setPosition(const vec3 &_position) {
    position = _position;
    updateViewTransform();
  }
  inline void Translate(float x, float y, float z) {
    position = position + right * x + up * y + forward * z;
    updateViewTransform();
  }
  inline void rotateEuler(float fYaw, float fPitch, float fRoll) {
    forward = forward * RotateEuler(fYaw, fPitch, fRoll);
    updateViewTransform();
  }
  inline void setOrientation(float fYaw, float fPitch, float fRoll) {
    forward = vec3{0, 0, 1} * RotateEuler(fYaw, fPitch, fRoll);
    updateViewTransform();
  }

protected:
  void updateViewTransform() {

    forward.normalize();

    up = up - forward * (forward * up);
    up.normalize();

    right = Cross(up, forward);
    //clang-format off
    mat4x4 mat;
    mat.m[0][0] = right.x;
    mat.m[1][0] = right.y;
    mat.m[2][0] = right.z;

    mat.m[0][1] = up.x;
    mat.m[1][1] = up.y;
    mat.m[2][1] = up.z;

    mat.m[0][2] = forward.x;
    mat.m[1][2] = forward.y;
    mat.m[2][2] = forward.z;
    float tx = -position * right;
    float ty = -position * up;
    float tz = -position * forward;

    mat.m[3][0] = tx;
    mat.m[3][1] = ty;
    mat.m[3][2] = tz;
    mat.m[3][3] = 1.0f;
    viewTransform = mat;
  }
};

struct PerspectiveCamera : public Camera {
  PerspectiveCamera(float _aspectRatio, float _fov, float _zNear, float _zFar) {

    Camera(_aspectRatio, _fov, _zNear, _zFar);
    aspectRatio = _aspectRatio;
    fov = _fov;
    zNear = _zNear;
    zFar = _zFar;
    projectionTransform = ClipPrespective(aspectRatio, fov, zNear, zFar);
  };
  inline float getAspectRation() { return aspectRatio; }
  inline float getFov() { return fov; }
  inline float getZNear() { return zNear; }
  inline float getZFar() { return zFar; }
};
struct OrthogonalCamera : public Camera {
  OrthogonalCamera(float _aspectRatio, float _fov, float _zNear, float _zFar) {

    Camera(_aspectRatio, _fov, _zNear, _zFar);
    aspectRatio = _aspectRatio;
    fov = _fov;
    zNear = _zNear;
    zFar = _zFar;

    projectionTransform = ClipOrthogonal(aspectRatio, fov, zNear, zFar);
  };
  inline float getAspectRation() { return aspectRatio; }
  inline float getFov() { return fov; }
  inline float getZNear() { return zNear; }
  inline float getZFar() { return zFar; }
};

} // namespace FireEngine
#endif
