/*
 * Created by MajesticWaffle on 4/27/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */

#ifndef MINICRAFT_MINICRAFT_H
#define MINICRAFT_MINICRAFT_H

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "geode.h"
#include "iostream"

#include "minicraft_entities.h"

extern std::string g_world_name;
//Entities



//Functions
Chunk* world_load_chunk(Coord2i coord);
void world_unload_chunk(Chunk* chunk);
#endif
