#ifndef _PERMITIVES_HPP
#define _PERMITIVES_HPP
#include <fstream>
#include <initializer_list>
#include <sstream>
#include <stdexcept>
#include <uml/mat4x4.h>
#include <uml/transform.h>
#include <uml/utils.h>
#include <uml/vec2.h>
#include <uml/vec3.h>
#include <uml/vec4.h>
#include <vector>

#define FE_CLAMP(val, MIN, MAX)                                                \
  val = val < MIN ? MIN : val;                                                 \
  val = val > MAX ? MAX : val
namespace FireEngine {
// very simple vertex class
struct vertex {
  vec3 position;
  vec2 uv;
  vec3 normal;

  vertex() = default;

  vertex(const vec3 &pos, vec2 &_uv, vec3 norm)
      : position{pos}, uv{_uv}, normal{norm} {}
  vertex(const vec3 &pos, vec2 &_uv) : position{pos}, uv{_uv} {}
  vertex(const vec3 &pos) : position{pos} {}

  vertex(std::initializer_list<float> list) {
    if (list.size() == 3) {
      auto it = list.begin();

      position.x = *it++;
      position.y = *it++;
      position.z = *it;
    }

    else if (list.size() == 5) {
      auto it = list.begin();

      position.x = *it++;
      position.y = *it++;
      position.z = *it++;
      uv.x = *it++;
      uv.y = *it;
    } else {
      throw new std::invalid_argument("Invalid Argument for vertex");
    }
  }
  vertex operator*(const mat4x4 &m) {
    vertex v;
    v.uv = uv;
    v.normal = normal;
    v.position = position * m;
    return v;
  }
};

struct triangle {
  vertex verticies[3];
  vec3 normal; // it is quite dumb to store a normal for a triangle but this is
               // part of the learning journey xD
  vec3 color = {1, 1, 1};
  triangle() = default;
  triangle(std::initializer_list<vertex> list) {
    if (list.size() == 3) {
      auto it = list.begin();

      verticies[0] = *it++;
      verticies[1] = *it++;
      verticies[2] = *it;
    } else {
      throw new std::invalid_argument("Invalid Argument for vertex");
    }
  }
  vec3 comptueNormal() {
    vec3 q0 = verticies[1].position - verticies[0].position;
    vec3 q1 = verticies[2].position - verticies[0].position;
    vec3 _normal = Cross(q0, q1);
    normal = _normal;
    normal.normalize();
    return normal;
  }
  triangle operator*(const mat4x4 &m) {
    triangle t;
    t.normal = normal;
    t.color = color;

    t.verticies[0] = this->verticies[0] * m;
    t.verticies[1] = this->verticies[1] * m;
    t.verticies[2] = this->verticies[2] * m;
    return t;
  }
};

struct Object3D {
  std::vector<triangle> triangles;
  mat4x4 transform;
  Object3D() { transform = IdentityMat4x4(); }
  Object3D(const vec3 &pos) { transform = TranslateXYZ(pos.x, pos.y, pos.z); }
  Object3D(float fYaw, float fPitch, float fRoll) {
    transform = RotateEuler(fYaw, fPitch, fRoll);
  }
  void setTransform(const mat4x4 &m) { transform = m; }
  mat4x4 getTransform(void) { return transform; }
  void setPosition(const vec3 &pos) {
    transform.m[3][0] = pos.x;
    transform.m[3][1] = pos.y;
    transform.m[3][2] = pos.z;
  }
  void setRotation(float fYaw, float fPitch, float fRoll) {
    float x = transform.m[3][0];
    float y = transform.m[3][1];
    float z = transform.m[3][2];

    transform = RotateEuler(fYaw, fPitch, fRoll);
    transform.m[3][0] = x;
    transform.m[3][1] = y;
    transform.m[3][2] = z;
  }

  void LoadFromObj(const char *path) {
    if (!triangles.empty())
      triangles.clear();
    std::fstream file = std::fstream(path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
      fprintf(stderr, "Couldn't open file\n");
      exit(1);
    }

    std::string junk;

    std::string line;
    std::vector<vertex> vertices;

    while (std::getline(file, line, '\n')) {
      if (line[0] == 'v') {
        // defining verticies
        std::stringstream stream(line);

        float x, y, z;
        stream >> junk >> x >> y >> z;

        vertices.push_back(vec3(x, y, z));
      }
      if (line[0] == 'f') {
        // loading face data assuming the faces are triangualted

        size_t id1, id2, id3;

        std::stringstream stream(line);
        stream >> junk >> id1 >> id2 >> id3;
        id1--;
        id2--;
        id3--;
        if (id1 < vertices.size() && id2 < vertices.size() &&
            id3 < vertices.size()) {
          this->triangles.push_back(
              {vertices[id1], vertices[id2], vertices[id3]});
        }
      }
    }
  }
};

std::vector<triangle> PlaneClipTriangle(vec3 plane_n, vec3 plane_p,
                                        triangle in_tri) {
  vertex insideVertices[3];
  vertex outsideVertcies[3];

  std::vector<triangle> clippedTriangles;
  size_t insideVerticesCount = 0;
  size_t outsideVertciesCount = 0;

  for (size_t i = 0; i < 3; i++) {
    if (PointPlaneDistance(plane_n, plane_p, in_tri.verticies[i].position) >=
        0) {
      insideVertices[insideVerticesCount++] = in_tri.verticies[i];
    } else {
      outsideVertcies[outsideVertciesCount++] = in_tri.verticies[i];
    }
  }

  if (insideVerticesCount == 3) {
    clippedTriangles.push_back(in_tri);
    return clippedTriangles;
  }

  else if (insideVerticesCount == 1 && outsideVertciesCount == 2) {
    vertex in_vert1 = insideVertices[0];
    vertex out_vert1 = outsideVertcies[0];
    vertex out_vert2 = outsideVertcies[1];

    triangle newtri;
    /* newtri.color = {1, 0, 0}; */
    newtri.verticies[0] = in_vert1;
    newtri.verticies[1] = vertex(LineIntersectPlane(
        plane_n, plane_p, in_vert1.position, out_vert1.position));
    newtri.verticies[2] = vertex(LineIntersectPlane(
        plane_n, plane_p, in_vert1.position, out_vert2.position));
    vec3 q0 = newtri.verticies[1].position - newtri.verticies[0].position;
    vec3 q1 = newtri.verticies[2].position - newtri.verticies[0].position;
    vec3 normal = Cross(q0, q1);
    normal.normalize();
    /* newtri.normal = normal; */

    clippedTriangles.push_back(newtri);
  } else if (insideVerticesCount == 2 && outsideVertciesCount == 1) {
    // forming a quad
    //
    triangle newtri1;
    triangle newtri2;
    newtri1.verticies[0] = insideVertices[0];

    newtri1.verticies[1] = insideVertices[1];

    /* newtri1.color = {0, 1, 0}; */
    /* newtri2.color = {0, 0, 1}; */
    vertex newvert1 =
        vertex(LineIntersectPlane(plane_n, plane_p, insideVertices[0].position,
                                  outsideVertcies[0].position));
    newtri1.verticies[2] = newvert1;

    newtri2.verticies[0] = insideVertices[1];
    newtri2.verticies[1] =
        vertex(LineIntersectPlane(plane_n, plane_p, insideVertices[1].position,
                                  outsideVertcies[0].position));
    newtri2.verticies[2] = newvert1;
    vec3 q0 = newtri1.verticies[1].position - newtri1.verticies[0].position;
    vec3 q1 = newtri1.verticies[2].position - newtri1.verticies[0].position;
    vec3 normal = Cross(q0, q1);
    normal.normalize();
    /* newtri1.normal = normal; */

    q0 = newtri2.verticies[1].position - newtri2.verticies[0].position;
    q1 = newtri2.verticies[2].position - newtri2.verticies[0].position;
    normal = Cross(q0, q1);
    normal.normalize();
    /* newtri2.normal = normal; */

    clippedTriangles.push_back(newtri1);
    clippedTriangles.push_back(newtri2);
  }

  return clippedTriangles;
}
} // namespace FireEngine
#endif