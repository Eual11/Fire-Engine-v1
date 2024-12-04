
# FireEngine v1
FireEngine is a C++ based rendering engine built with SDL for fragment rasterization. It serves as a personal learning project to explore various concepts in graphics programming, 3D rendering, and engine architecture. This engine is designed to experiment with rendering techniques and improve understanding of how graphics engines work at a fundamental level.

![C++](https://img.shields.io/badge/C%2B%2B-%231572B6.svg?style=flat&logo=c%2B%2B&logoColor=white)
![SDL2](https://img.shields.io/badge/SDL2-%2333CCFF.svg?style=flat&logo=SDL&logoColor=white)

## Features

- **3D Rendering:** Implements basic 3D rendering, including perspective projections and object transformations.
- **Camera System:** Includes a camera system for navigation and viewing in 3D space.
- **Lighting:** Supports directional lighting for simulating light sources in the scene.
- **Texture Mapping:** Implements basic texture mapping using SDL for displaying images on 3D objects.
- **Backface Culling:** Optionally disables backface culling for rendering efficiency.
- **Wireframe Rendering:** Includes support for wireframe rendering mode for visualizing object meshes.
- **Frustum Clipping:** Implements near and far plane clipping for object visibility within the camera view.


## Installation

### Prerequisites
Before running or building the project, ensure you have the following installed:
- C++ Compiler (GCC, Clang, MSVC, etc.)
- SDL2 (with SDL_image, SDL_ttf)
- make
## Dependencies
- **SDL2** for rendering and window management.
- **Unkown_Mathlib** (my own math library) for vector, matrix, and transformation operations. [GitHub link](https://github.com/Eual11/Unkown_Mathlib)
### Building and Running

1. Clone the repository:
    ```bash
    git clone https://github.com/Eual11/Fire-Engine-v1/
    ```

2. Install dependencies:
    - On Ubuntu/Debian:
      ```bash
      sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
      ```

    - On Windows, use a package manager like `vcpkg` or `MSYS2` to install SDL2 and related libraries.

3. Build the project:
    ```bash
    make
    ```

4. Run the demo:
    ```bash
    ./build/FireEngine
    ```
## Usage
In the demo, the camera can be controlled with the following keys:
- `W`, `A`, `S`, `D` to move the camera.
- `UP`, `DOWN` to move the camera vertically.
- `E`, `R` to rotate the camera along the Y-axis.
- `F`, `V` to rotate the camera along the X-axis.


## Screenshots

Here are some examples of scenes rendered with **FireEngine**:

![Rendering Example 1](./snapshots/5-29-2024/Screenshot%202024-05-29%20014928.png)

*An example scene showcasing 3D models rendered with FireEngine.*

![Rendering Example 2](./snapshots/5-29-2024/Screenshot%202024-05-29%20011831.png)
*Another view of the rendered scene with different camera angles and lighting.*

![Lighting and Shadows](./snapshots/5-29-2024/Screenshot%202024-12-04%20120849.png)
*Demonstration of directional lighting effects on a 3D objects.*

![Texturing objects](./snapshots/6-5-2024/Screenshot%202024-06-05%20005736.png)
*Texture mapping demo.*

# Demo
Loading a level from Sypro the dragon(PS1) on FireEngine:

![FireEngine Demo](./snapshots/demo_vids/demo.gif)


## Contributing

Contributions are always welcome!



## License

[MIT](https://choosealicense.com/licenses/mit/)

