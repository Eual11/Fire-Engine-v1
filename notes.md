## Fire Engine 
- currently only capable of rendering a cube pretty neatly xD 
- on an attempt to add texutures, the texuture coordinates are not only wrong but there is a bad stretching and 
the texture may dissapear ! 
- [X] invisble surface removal, simple one, yeet any triangle that face outward 
- something is wrong with the right face, it is slightely slanted, and the texture is completely wrong 
- in general there is a huge problem with the renderer that caused weird slanting and mis-scalling errors when the object is moved to the sides
``` cpp 
  mesh cube;
  cube.tris = {// FRONT FACE
               {{0, 1, 0}, {1, 1, 0}, {0, 0, 0}},
               {{1, 0, 0}, {0, 0, 0}, {1, 1, 0}},

               /* // BACK */
               {{1, 0, 1}, {1, 1, 1}, {0, 1, 1}},
               {{1, 0, 1}, {0, 1, 1}, {0, 0, 1}},

               // LFFT
               {{0, 1, 1}, {0, 1, 0}, {0, 0, 1}},
               {{0, 1, 0}, {0, 0, 0}, {0, 0, 1}},

               // RIGHT
               {{1, 1, 0}, {1, 1, 1}, {1, 0, 1}},
               {{1, 0, 1}, {1, 0, 0}, {1, 1, 0}},

               // TOP
               {{0, 1, 0}, {0, 1, 1}, {1, 1, 1}},
               {{1, 1, 1}, {1, 1, 0}, {0, 1, 0}},

               // BOTTOM
               {{0, 0, 0}, {1, 0, 1}, {0, 0, 1}},
               {{1, 0, 1}, {0, 0, 0}, {1, 0, 0}}

  };
```
FRONT face normal: (0.000000, 0.000000, -1.000000)
FRONT face normal: (0.000000, 0.000000, -1.000000)
BACK face normal: (0.000000, -0.000000, 1.000000)
BACK face normal: (0.000000, 0.000000, 1.000000)



``` cpp 
mat4x4 MakeLookAt(vec3 camerapos, vec3 up, vec3 target) {
  vec3 forward = target - camerapos;
  forward.normalize();

  vec3 newup = up - (forward * (forward * up));
  newup.normalize();

  vec3 right = Cross(newup, forward);

  float tx = camerapos * right;
  float ty = newup * camerapos;
  float tz = forward * camerapos;

  mat4x4 m{right.x,     right.y,     right.z,     0,         newup.x,   newup.y,
           newup.z,     0,           forward.x,   forward.y, forward.z, 0,
           camerapos.x, camerapos.y, camerapos.z, 1};

  return test(m);
}
mat4x4 test(mat4x4 mat) {
  mat4x4 matrix;
  matrix[0][0] = mat[0][0];
  matrix[0][1] = mat[1][0];
  matrix[0][2] = mat[2][0];
  matrix[0][3] = 0;
  matrix[1][0] = mat[0][1];
  matrix[1][1] = mat[1][1];
  matrix[1][2] = mat[2][1];
  matrix[1][3] = 0;
  matrix[2][0] = mat[0][2];
  matrix[2][1] = mat[1][2];
  matrix[2][2] = mat[2][2];
  matrix[2][0] = 0;
  matrix[3][0] = -(mat[3][0] * matrix[0][0] + mat[3][1] * matrix[1][0] +
                   mat[3][2] * matrix[2][0]);
  matrix[3][1] = -(mat[3][0] * matrix[0][1] + mat[3][1] * matrix[1][1] +
                   mat[3][2] * matrix[2][1]);
  matrix[3][2] = -(mat[3][0] * matrix[0][2] + mat[3][1] * matrix[1][2] +
                   mat[3][2] * matrix[2][2]);
  matrix[3][3] = 1.0f;
  return matrix;
}

```


### Current State
- 5/27/2024
- currently the "engine" only supports a single object being renderered with a very simple light and movable camera. 
so the whole application can be abstracted as a world, an object to be rendererd, a directional light source, and a simple camera and a renderer that renders everything to the back buffer
- 
