#ifndef _FIRE_ENGINE_RENDERER_HPP
#define _FIRE_ENGINE_RENDERER_HPP

#include "Camera.hpp"
#include "World.hpp"
#include <SDL2/SDL_render.h>
#include <algorithm>
#include <list>
#include <uml/vec4.h>
enum FireEngine_RendererFlags {
  FireEngine_RendererFlags_None = 0,
  FireEngine_RendererFlags_DisableBackfaceCulling =
      1 << 1, // flag to disable backface culling
  FireEngine_RendererFlags_RenderWireframe = 1 << 2,
};
namespace FireEngine {

class Renderer {
public:
  Renderer() = default;
  FireEngine_RendererFlags flags;
  Renderer(SDL_Renderer *_renderer, SDL_Rect _viewport,
           FireEngine_RendererFlags _flags = FireEngine_RendererFlags_None)
      : m_Renderer{_renderer} {

    m_ViewPort = _viewport;
    flags = _flags;
  }
  inline void setViewport(SDL_Rect _viewport) { m_ViewPort = _viewport; }
  inline void setRenderer(SDL_Renderer *_renderer) { m_Renderer = _renderer; }

  inline SDL_Renderer *getRenderer(void) { return m_Renderer; }
  inline SDL_Rect getViewport(void) { return m_ViewPort; }

  void Render(World world, Camera camera) {
    for (auto &object : world.ObjectList) {
      std::vector<triangle> projectedMesh;
      for (auto tri : object->triangles) {

        // world transform
        tri = tri * object->getTransform();
        vec3 normal = tri.comptueNormal();
        float theta = normal * (tri.verticies[0].position - camera.position);
        // backface culling
        if (theta < 0 ||
            flags & FireEngine_RendererFlags_DisableBackfaceCulling) {
          // view transform
          tri = tri * camera.getViewTransform();
          // near plane clipping
          std::vector<triangle> clippedTriangles =
              PlaneClipTriangle({0, 0, 1}, {0, 0, camera.zNear}, tri);
          // TODO: maybe add far plane clipping
          //
          triangle projectedTriangle;

          for (auto &clippedTri : clippedTriangles) {
            for (size_t i = 0; i < 3; i++) {
              vec4 projectedVec = vec4{clippedTri.verticies[i].position, 1};
              projectedVec = projectedVec * camera.getProjectionTransform();
              // prespective divide
              //
              if (projectedVec.w != 0.0f) {
                projectedVec.x /= projectedVec.w;
                projectedVec.y /= projectedVec.w;
                projectedVec.z /= projectedVec.w;
              }

              vertex projectedVertex;
              projectedVertex.position.x = projectedVec.x;

              projectedVertex.position.y = projectedVec.y;
              projectedVertex.position.z = projectedVec.z;
              projectedTriangle.verticies[i] = projectedVertex;
            }

            projectedTriangle.normal = normal;
            projectedTriangle.color = clippedTri.color;

            std::list<triangle> listTriangles;
            listTriangles.push_back(projectedTriangle);
            uint8_t newTrianglesCount = 1;

            for (uint8_t p = 0; p < 4; p++) {
              while (newTrianglesCount > 0) {
                triangle test = listTriangles.front();
                std::vector<triangle> newTriangles;
                listTriangles.pop_front();
                newTrianglesCount--;

                switch (p) {
                case 0: {
                  // left screen edge
                  newTriangles =
                      PlaneClipTriangle({1.0, 0, 0}, {-1.0, 0, 0}, test);
                  break;
                }
                case 1: {
                  // top screen edge

                  newTriangles =
                      PlaneClipTriangle({0, -1, 0}, {0, 1.0, 0}, test);
                  break;
                }
                case 2: {
                  // right screen edge
                  newTriangles =
                      PlaneClipTriangle({-1, 0, 0}, {1.0, 0, 0}, test);
                  break;
                }
                case 3: {
                  // bottom screen edge
                  newTriangles =
                      PlaneClipTriangle({0, 1, 0}, {0, -1.0, 0}, test);
                  break;
                }
                }

                for (auto &tr : newTriangles) {
                  listTriangles.push_back(tr);
                }
              }
              newTrianglesCount = listTriangles.size();
            }

            for (auto &tr : listTriangles) {
              tr.normal = normal;
              projectedMesh.push_back(tr);
            }
          }
        }
      }
      // TODO: implement a depth buffer
      //  painters algorithm
      std::sort(projectedMesh.begin(), projectedMesh.end(),
                [](const triangle &a, const triangle &b) {
                  float z1 =
                      (a.verticies[0].position.z + a.verticies[1].position.z +
                       a.verticies[2].position.z) /
                      3;
                  float z2 =
                      (b.verticies[0].position.z + b.verticies[1].position.z +
                       b.verticies[2].position.z) /
                      3;

                  return z1 > z2;
                });

      std::vector<SDL_Vertex> vertexToRender;
      size_t projectedMeshTriCount = projectedMesh.size();
      vertexToRender.resize(projectedMeshTriCount * 3);
      size_t k = 0;
      for (auto &tri : projectedMesh) {

        for (int j = 0; j < 3; j++) {
          const vertex &vert = tri.verticies[j];
          SDL_Vertex finalvert;

          finalvert.position.x =
              m_ViewPort.x + (vert.position.x + 1) * 0.5 * m_ViewPort.w;
          finalvert.position.y = m_ViewPort.y + m_ViewPort.h -
                                 (vert.position.y + 1) * 0.5 * m_ViewPort.h;
          // calculating basic lambertain lighting using per-triangle normals,
          // this will obviously result in a very janky flat shading, but this
          // is part of the learnign process
          vec3 ln{0.0, 0.0, 0.0};
          for (auto &light : world.Lights) {
            ln += light->calculateLambertian(vert.position, tri.normal);
          }
          // maybe we need to clamp each color value from 0 to 1?
          vec3 color = (ln & tri.color) * 255.0f;
          finalvert.color = {static_cast<uint8_t>(color.x),
                             static_cast<uint8_t>(color.y),
                             static_cast<uint8_t>(color.z), 0xff};

          /* finalvert.tex_coord = {0}; */
          tri.verticies[j].position.x = finalvert.position.x;
          tri.verticies[j].position.y = finalvert.position.y;

          vertexToRender[k++] = finalvert;
        }
        //    tri.verticies
        if (FireEngine_RendererFlags_RenderWireframe & flags) {
          SDL_SetRenderDrawColor(m_Renderer, 0xff * tri.color.x,
                                 0xff * tri.color.y, 0xff * tri.color.z, 0xFF);

          SDL_RenderDrawLineF(m_Renderer, tri.verticies[0].position.x,
                              tri.verticies[0].position.y,
                              tri.verticies[1].position.x,
                              tri.verticies[1].position.y);
          SDL_RenderDrawLineF(m_Renderer, tri.verticies[0].position.x,
                              tri.verticies[0].position.y,
                              tri.verticies[2].position.x,
                              tri.verticies[2].position.y);
          SDL_RenderDrawLineF(m_Renderer, tri.verticies[2].position.x,
                              tri.verticies[2].position.y,
                              tri.verticies[1].position.x,
                              tri.verticies[1].position.y);
        }
      }
      if (!(flags & FireEngine_RendererFlags_RenderWireframe)) {
        SDL_RenderGeometry(m_Renderer, nullptr, vertexToRender.data(),
                           vertexToRender.size(), nullptr, 0);
      }
    }
  }

private:
  SDL_Renderer *m_Renderer;
  SDL_Rect m_ViewPort;
};
} // namespace FireEngine

#endif