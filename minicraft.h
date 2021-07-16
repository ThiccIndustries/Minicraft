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

extern std::string g_world_name;
//Entities

//Player
typedef struct Entity_Player{
    Entity  e{
            .bounds = {{5,10}, {11, 15}},
            .animation_rate = TIME_TPS,
            .type = ENT_PLAYER
    };
    uint    health = 5;
}Entity_Player;


//Functions
Chunk* world_load_chunk(Coord2i coord);
void world_unload_chunk(Chunk* chunk);
#endif
