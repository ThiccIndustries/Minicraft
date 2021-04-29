cmake_minimum_required(VERSION 3.20)
project(Minicraft)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

find_package(OpenGL REQUIRED)
add_executable(Minicraft minicraft.h minicraft_main.cpp minicraft_rendering.cpp minicraft_texture.cpp minicraft_world.cpp minicraft_input.cpp minicraft_time.cpp minicraft_entity.cpp minicraft_player.cpp)
target_link_libraries(Minicraft -lGLEW -lglfw -lGL)