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
#include "geode_uiextensions.h"
#include "geode_ntn.h"

#include "iostream"

#define TILE_COLLECTABLE TILE_LAST << 1

//Fuck header files

typedef struct Save{
    int seed;
    Coord2i player_position;
    Map* map;
} Save;

extern Save*      g_save;

//Functions
Chunk*  world_load_chunk(Map* map, Coord2i coord);
void    world_unload_chunk(Map* map, Chunk* chunk);

Save*   world_load_game(uint id);
void world_save_game(Save *save);

//Entities
#include "minicraft_components.h"
#endif
