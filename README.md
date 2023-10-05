# Ray Tracing
Continuation of my previous [Ray Tracing](https://github.com/1neskk/raytracing) project
![image](https://github.com/1neskk/RayTracing2/assets/113075816/f2b1e50c-11c6-426c-812b-c89a88347e63)

**Officially supports Windows 10/11 and Visual Studio 2022.**

## Requirements
- [Vulkan SDK](https://vulkan.lunarg.com/)
- ImGui and GLFW binaries are not included in this repository.

### Get Started with CMake
1. Clone recursively: `git clone --recursive https://github.com/1neskk/RayTracing2`
2. Change branch to "cmake" branch using `git checkout cmake` on your cmd
3. Then run `mkdir build && cd build` to create a new directory
4. Finally run `cmake ..` to build with CMake

### Get Started with Premake (Windows only)
1. Clone recursively: `git clone --recursive https://github.com/1neskk/RayTracing2`
2. Run `script/setup.bat` to generate the solution and project files
3. Then open RayTracing.sln and hit F5 (preferably change configuration to Release first, for the sake of performance)
