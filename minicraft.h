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

enum EntityType{
    ENT_GENERIC,
    ENT_PLAYER,
    ENT_ZOMBIE
};
//Player
typedef struct Entity_Player{
    Entity  e{
            .atlas_index = 0,
            .bounds = {{5,10}, {11, 15}},
            .animation_rate = TIME_TPS,
            .type = ENT_PLAYER
    };

    double movementSpeed = ((3.0 * 16) / TIME_TPS);
    uint health = 5;
}Entity_Player;

typedef struct Entity_Zombie{
    Entity e{
        .atlas_index = 3,
        .bounds = {{5, 10}, {11, 15}},
        .animation_rate = TIME_TPS,
        .type = ENT_ZOMBIE
    };

    double movementSpeed = ((1.5 * 16) / TIME_TPS);
    uint health = 10;
    uint attack_range = 1 * 16;
    uint follow_range = 5 * 16;
    Entity* target = nullptr;
}Entity_Zombie;


//Functions
Chunk* world_load_chunk(Coord2i coord);
void world_unload_chunk(Chunk* chunk);
#endif
