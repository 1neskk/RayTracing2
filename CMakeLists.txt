cmake_minimum_required(VERSION 3.17)

project(RayTracingEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(CUDA REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_executable(RayTracingEngine ${SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw/include)
target_link_libraries(RayTracingEngine ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw3.lib)

target_link_libraries(RayTracingEngine ${CMAKE_CURRENT_SOURCE_DIR}/libs/ImGui.lib)
target_link_libraries(RayTracingEngine opengl32.lib)

include_directories(${Vulkan_INCLUDE_DIRS})
target_link_libraries(RayTracingEngine ${Vulkan_LIBRARIES})

set_target_properties(RayTracingEngine PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

# msbuild CudaRT.sln /p:Configuration=Release /p:Platform=x64
