cmake_minimum_required(VERSION 3.20)
project(Minicraft)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")


find_package(OpenGL REQUIRED)
add_executable(Minicraft minicraft.h minicraft_main.cpp minicraft_rendering.cpp minicraft_texture.cpp minicraft_world.cpp minicraft_input.cpp minicraft_time.cpp minicraft_entity.cpp minicraft_storage.cpp)
target_link_libraries(Minicraft -lGLEW -lglfw -lGL)

file(MAKE_DIRECTORY resources)
add_custom_command( TARGET Minicraft POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                            ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources)