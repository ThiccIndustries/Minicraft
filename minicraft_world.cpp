/*
 * Created by MajesticWaffle on 4/26/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */

#include "minicraft.h"
#include <filesystem>

Chunk* g_chunk_buffer[RENDER_DISTANCE * RENDER_DISTANCE * 4];

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

Item* g_item_registry[255] = {
        nullptr,    //g_item_registry[0] reserved for empty storage slots

};

void world_populate_chunk_buffer(const std::string& savename, Entity* viewport_e){

    int player_chunk_x = (int)(viewport_e -> position.x + (viewport_e -> camera.position.x)) / 256;
    int player_chunk_y = (int)(viewport_e -> position.y + (viewport_e -> camera.position.x)) / 256;

    for(int x = 0; x < RENDER_DISTANCE * 2; x++){
        for(int y = 0; y < RENDER_DISTANCE * 2; y++){

            int chunk_x = player_chunk_x - (x - RENDER_DISTANCE);
            int chunk_y = player_chunk_y - (y - RENDER_DISTANCE);
            int chunki = x + (y * RENDER_DISTANCE * 2);

            if(g_chunk_buffer[chunki] != nullptr && g_chunk_buffer[chunki]->pos.x == chunk_x && g_chunk_buffer[chunki]->pos.y == chunk_y)
                continue;

            //Unload chunk already in buffer slot
            if(g_chunk_buffer[chunki] != nullptr)
                world_unload_chunk(savename, g_chunk_buffer[chunki]);

            g_chunk_buffer[chunki] = world_load_chunk(savename, Coord2i{chunk_x, chunk_y}, 0);
        }
    }
}

Chunk* world_load_chunk(const std::string& savename, Coord2i coord, int seed){
    Chunk* chunkptr = world_chunkfile_read(savename, coord );

    //Chunk successfully loaded from file
    if(chunkptr != nullptr) return chunkptr;

    chunkptr = new Chunk;

    //Generate chunk

    for(int tx = 0; tx < 16; tx++){
        for(int ty = 0; ty < 16; ty++){
            chunkptr -> foreground_tiles[(ty * 16) + tx] = 0;
        }
    }

    //Seed = x^3 + y
    /*std::srand((coord.x * coord.x * coord.x) + coord.y);

    int x1 = ((double)rand() / RAND_MAX) * 16;
    int y1 = ((double)rand() / RAND_MAX) * 16;


    chunkptr -> foreground_tiles[(y1 * 16) + x1] = 1;
    */
    chunkptr -> pos = coord;

    world_chunkfile_write(savename, chunkptr);

    return chunkptr;
}

void world_unload_chunk(const std::string& savename, Chunk* chunk){
    //TODO: save chunks to file
    world_chunkfile_write(savename, chunk);
    delete chunk;
}

void world_modify_chunk(const std::string& savename, Coord2i ccoord, Coord2i tcoord, uint value){
    //Look for chunk in chunk buffer
    Chunk* chunkptr = world_get_chunk(ccoord);

    //Chunk isn't loaded
    if(chunkptr == nullptr){
        return;
    }

    chunkptr -> foreground_tiles[tcoord.x + (tcoord.y * 16)] = value;

    world_chunkfile_write(savename, chunkptr);
}


Chunk* world_get_chunk(Coord2i ccoord){
    for(Chunk* c : g_chunk_buffer){
        if(c -> pos.x == ccoord.x && c -> pos.y == ccoord.y){
            return c;
        }
    }
    return nullptr;
}

Chunk* world_chunkfile_read(const std::string& savename, Coord2i coord){
    FILE* chunkfile = fopen(get_resource_path(g_game_path, "saves/" + savename + "/chunks/c" + std::to_string(coord.x) + "x" + std::to_string(coord.y) + ".cf").c_str(), "rb");

    //Chunk is new (no chunkfile)
    if(!chunkfile){
        return nullptr;
    }

    Chunk* chunkptr = new Chunk;

    fread(&chunkptr -> pos.x, 1, sizeof(int), chunkfile);
    fread(&chunkptr -> pos.y, 1, sizeof(int), chunkfile);

    fseek(chunkfile, 0x10, SEEK_SET);

    fread(chunkptr -> overlay_tiles,    256, sizeof(uchar), chunkfile);
    fread(chunkptr -> foreground_tiles, 256, sizeof(uchar), chunkfile);

    fclose(chunkfile);

    return chunkptr;
}

void world_chunkfile_write(const std::string& savename, Chunk* chunk){
    int cx = chunk -> pos.x;
    int cy = chunk -> pos.y;

    std::filesystem::create_directories(get_resource_path(g_game_path, "saves/" + savename + "/chunks"));
    FILE* chunkfile = fopen(get_resource_path(g_game_path, "saves/" + savename + "/chunks/c" + std::to_string(cx) + "x" + std::to_string(cy) + ".cf").c_str(), "wb");

    fwrite(&cx, sizeof(int), 1, chunkfile);
    fwrite(&cy, sizeof(int), 1, chunkfile);

    fseek(chunkfile, 0x10, SEEK_SET);

    fwrite(chunk -> overlay_tiles,      sizeof(uchar), 256, chunkfile);
    fwrite(chunk -> foreground_tiles,   sizeof(uchar), 256, chunkfile);

    fclose(chunkfile);
}
