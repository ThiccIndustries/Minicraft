/*
 * Created by MajesticWaffle on 7/13/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */

#include "minicraft.h"

Block* g_block_registry[255] = {
        /*        Block Definitions------------------------------------------------------
                  Texture   Options                         Material    DropID  DropCount */
        new Block{0,        0,                              MAT_EARTH,  0,      0}, //Grass
        new Block{16,       0,                              MAT_EARTH,  0,      0}, //Grass_rose
        new Block{1,        0,                              MAT_STONE,  0,      0}, //Stone
        new Block{2,        TILE_SOLID,                     MAT_SOLID,  0,      0}, //water
        new Block{10,       TILE_SOLID,                     MAT_SOLID,  0,      0}, //water_lily

        new Block{3 ,       TILE_SOLID,                     MAT_SOLID,  0,      0}, //water_grass_TL
        new Block{4 ,       TILE_SOLID,                     MAT_SOLID,  0,      0}, //water_grass_TC
        new Block{3 ,       TILE_SOLID | TILE_TEX_FLIP_X,   MAT_SOLID,  0,      0}, //water_grass_TR

        new Block{5,        TILE_SOLID,                     MAT_SOLID,  0,      0},  //water_grass_MR
        new Block{5,        TILE_SOLID | TILE_TEX_FLIP_X,   MAT_SOLID,  0,      0},  //water_grass_ML

        new Block{6,        TILE_SOLID,                     MAT_SOLID,  0,      0},  //water_grass_BL
        new Block{7,        TILE_SOLID,                     MAT_SOLID,  0,      0},  //water_grass_BC
        new Block{6,        TILE_SOLID | TILE_TEX_FLIP_X,   MAT_SOLID,  0,      0},  //water_grass_BR

        new Block{11 ,      TILE_SOLID,                     MAT_SOLID,  0,      0}, //water_stone_TL
        new Block{12 ,      TILE_SOLID,                     MAT_SOLID,  0,      0}, //water_stone_TC
        new Block{11 ,      TILE_SOLID | TILE_TEX_FLIP_X,   MAT_SOLID,  0,      0}, //water_stone_TR

        new Block{13,       TILE_SOLID,                     MAT_SOLID,  0,      0},  //water_stone_MR
        new Block{13,       TILE_SOLID | TILE_TEX_FLIP_X,   MAT_SOLID,  0,      0},  //water_stone_ML

        new Block{14,       TILE_SOLID,                     MAT_SOLID,  0,      0},  //water_stone_BL
        new Block{15,       TILE_SOLID,                     MAT_SOLID,  0,      0},  //water_stone_BC
        new Block{14,       TILE_SOLID | TILE_TEX_FLIP_X,   MAT_SOLID,  0,      0},  //water_stone_BR

        new Block{8,        TILE_SOLID,                     MAT_WOOD,   0,      0}, //Resource_Wood
        new Block{9,        TILE_SOLID,                     MAT_STONE,  0,      0}, //Resource_Stone
};

Chunk* world_load_chunk(Coord2i coord){
    Chunk* chunkptr = world_chunkfile_read("saves/" + g_world_name + "/chunks", coord );

    //Chunk successfully loaded from file
    if(chunkptr != nullptr) return chunkptr;

    chunkptr = new Chunk;

    //Generate chunk

    for(int tx = 0; tx < 16; tx++){
        for(int ty = 0; ty < 16; ty++){
            chunkptr -> foreground_tiles[(ty * 16) + tx] = 0;
        }
    }

    chunkptr -> pos = coord;
    world_chunkfile_write("saves/" + g_world_name + "/chunks", chunkptr);

    return chunkptr;
}

void world_unload_chunk(Chunk* chunk){
    //TODO: save chunks to file
    world_chunkfile_write("saves/" + g_world_name + "/chunks", chunk);
    delete chunk;
}