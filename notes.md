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

