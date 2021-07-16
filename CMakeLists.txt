cmake_minimum_required(VERSION 3.20)
project(Minicraft)

include_directories(${CMAKE_SOURCE_DIR}\\Geode\\)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")


find_package(OpenGL REQUIRED)
add_executable(Minicraft minicraft.h minicraft_main.cpp minicraft_world.cpp)
target_link_libraries(Minicraft -lGLEW -lglfw -lGL Geode-Lib)

file(MAKE_DIRECTORY resources)
add_custom_command( TARGET Minicraft PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                            ${CMAKE_SOURCE_DIR}/Minicraft/resources ${CMAKE_BINARY_DIR}/Minicraft/resources)