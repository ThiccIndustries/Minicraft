cmake_minimum_required(VERSION 3.20)
project(Minicraft)

include_directories(${CMAKE_SOURCE_DIR}/Geode-Engine/)
add_subdirectory(${CMAKE_SOURCE_DIR}/Geode-Engine/)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

add_executable(Minicraft minicraft.h minicraft_main.cpp minicraft_world.cpp minicraft_entities.h minicraft_entities.cpp)
target_link_libraries(Minicraft Geode-Lib)

add_custom_command( TARGET Minicraft PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                            ${CMAKE_SOURCE_DIR}/resources ${PROJECT_BINARY_DIR}/resources)