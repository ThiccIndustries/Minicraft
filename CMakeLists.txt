cmake_minimum_required(VERSION 3.20)
project(Minicraft)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/GLFW)
add_subdirectory(${CMAKE_SOURCE_DIR}/glad-master)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

include_directories(${CMAKE_SOURCE_DIR}/GLFW/include ${CMAKE_SOURCE_DIR}/glad-master/include/)

find_package(OpenGL REQUIRED)
add_executable(Minicraft minicraft.h minicraft_main.cpp minicraft_rendering.cpp mincraft_texture.cpp minicraft_world.cpp PerlinNoise.hpp)
target_link_libraries(Minicraft glad glfw)