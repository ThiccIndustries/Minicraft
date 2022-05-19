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

#define TILE_COLLECTABLE TILE_LAST << 1

//Entities
#include "minicraft_entities.h"

//Structures
typedef struct Structure{
    Coord2i size;
    uchar* tiles;
} Structure;

typedef struct Save_Data{
    Coord2i player_position;
    long seed;
} Save_Data;

typedef struct Save{
    Save_Data s;
    Map* overworld;
} Save;

typedef struct Item{
    uint atlas_index;

} Item;

extern Save*      g_save;
extern Structure* g_structures[255];

//Functions
Chunk*  world_load_chunk(Map* map, Coord2i coord);
void    world_unload_chunk(Map* map, Chunk* chunk);

Save*   world_load_game(uint id);
void    world_save_game(Save* save);


#endif
