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

#define TILE_COLLECTABLE 0b00001000

//Entities
#include "minicraft_entities.h"

//Structures
typedef struct Structure{
    Coord2i size;
    uchar* tiles;
} Structure;

typedef struct Save{
    std::string world_name;
    Coord2i player_position;
    long seed;
};

extern Save*      g_save;
extern Structure* g_structures[255];

//Functions
Chunk*  world_load_chunk(Coord2i coord);
void    world_unload_chunk(Chunk* chunk);
Save*   world_load_game(const std::string& world_name);
void    world_save_game(Save* save);
#endif
