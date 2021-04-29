/*
 * Created by MajesticWaffle on 4/26/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

Chunk* ChunkBuffer[RENDER_DISTANCE * RENDER_DISTANCE * 4];

Block* BlockRegistry[255] = {
        world_construct_block(0, 0, MAT_EARTH, 0, 0),   //Grass
        world_construct_block(2, 0, MAT_STONE, 0, 0),   //Stone
        world_construct_block(8, 0, MAT_EARTH, 0, 0),   //Sand

        world_construct_block(4, 0, MAT_SOLID, 0, 0),   //Water
        world_construct_block(5, 0, MAT_SOLID, 0, 0),   //Water Grass
        world_construct_block(6, 0, MAT_SOLID, 0, 0),  //Water Stone
        world_construct_block(7, 0, MAT_SOLID, 0, 0),   //Water Sand
};

Item* ItemRegistry[255] = {
        nullptr,    //ItemRegistry[0] reserved for empty storage slots

};

void world_populate_chunk_buffer(std::string savename, Camera* cam){

    int player_chunk_x = (int)cam -> position.x / 256;
    int player_chunk_y = (int)cam -> position.y / 256;

    for(int x = 0; x < RENDER_DISTANCE * 2; x++){
        for(int y = 0; y < RENDER_DISTANCE * 2; y++){

            int chunk_x = player_chunk_x - (x - RENDER_DISTANCE);
            int chunk_y = player_chunk_y - (y - RENDER_DISTANCE);
            int chunki = x + (y * RENDER_DISTANCE * 2);

            if(ChunkBuffer[chunki] != nullptr && ChunkBuffer[chunki]->pos_x == chunk_x && ChunkBuffer[chunki]->pos_y == chunk_y)
                continue;

            //Unload chunk already in buffer slot
            if(ChunkBuffer[chunki] != nullptr)
                world_unload_chunk(savename, ChunkBuffer[chunki]);

            ChunkBuffer[chunki] = world_load_chunk(savename, chunk_x, chunk_y, 0);
        }
    }
}

Chunk* world_load_chunk(std::string savename, int x, int y, int seed){
    Chunk* chunkptr = world_chunkfile_read(savename, x, y);

    //Chunk successfully loaded from file
    if(chunkptr != nullptr) return chunkptr;

    chunkptr = new Chunk;
    //Chunkfile does not exist
    double perlin_coord_y1, perlin_coord_x, perlin_coord_y;

    for(int tx = 0; tx < 16; tx++){
        for(int ty = 0; ty < 16; ty++){
            perlin_coord_x = ((x * 256) + tx) * WORLD_PERLIN_SCALE;

            perlin_coord_y = ((x * 256) + ty) * WORLD_PERLIN_SCALE;
            perlin_coord_y1 = ((x * 256) + (ty - 1)) * WORLD_PERLIN_SCALE;

            chunkptr -> foreground_tiles[(ty * 16) + tx] = (x + y) % 2 == 0 ? 0 : 2;

        }
    }

    chunkptr -> pos_x = x;
    chunkptr -> pos_y = y;
    return chunkptr;
}

void world_unload_chunk(std::string savename, Chunk* chunk){
    //TODO: save chunks to file
    world_chunkfile_write(savename, chunk);
    delete chunk;
}

void world_modify_chunk(std::string savename, int cx, int cy, int tx, int ty, int value){
    //Look for chunk in chunk buffer
    Chunk* chunkptr;
    for(Chunk* c : ChunkBuffer){
        if(c -> pos_x == cx && c -> pos_y == cy){
            chunkptr = c;
            break;
        }
    }

    //Chunk isn't loaded
    if(chunkptr == nullptr){
        return;
    }

    chunkptr -> foreground_tiles[tx + (ty * 16)] = value;

    world_chunkfile_write(savename, chunkptr);
}

Block* world_construct_block(uint id, uchar options, Material mat, uint drop_id, uint drop_count){
    Block* blockptr = new Block;

    blockptr -> atlas_index = id;
    blockptr -> options     = options;
    blockptr -> material    = mat;
    blockptr -> drop_id     = drop_id;
    blockptr -> drop_count  = drop_count;

    return blockptr;
}

Chunk* world_chunkfile_read(std::string savename, uint cx, uint cy){
    int pos_x, pos_y;
    uint overlay_tiles[256];
    uint tiles[256];

    FILE* chunkfile = fopen(get_resource_path(game_root_path, "saves/" + savename + "/chunks/c" + std::to_string(cx) + "x" + std::to_string(cy) + ".cf").c_str(), "rb");

    //Chunk is new (no chunkfile)
    if(!chunkfile){
        return nullptr;
    }

    Chunk* chunkptr = new Chunk;

    fread(&chunkptr -> pos_x, 1, sizeof(int), chunkfile);
    fread(&chunkptr -> pos_y, 1, sizeof(int), chunkfile);

    fseek(chunkfile, 0x10, SEEK_SET);

    fread(chunkptr -> overlay_tiles,    256, sizeof(uint), chunkfile);
    fread(chunkptr -> foreground_tiles, 256, sizeof(uint), chunkfile);

    fclose(chunkfile);

    return chunkptr;
}

void world_chunkfile_write(std::string savename, Chunk* chunk){
    int cx = chunk -> pos_x;
    int cy = chunk -> pos_y;

    FILE* chunkfile = fopen(get_resource_path(game_root_path, "saves/" + savename + "/chunks/c" + std::to_string(cx) + "x" + std::to_string(cy) + ".cf").c_str(), "wb");

    fwrite(&cx, sizeof(int), 1, chunkfile);
    fwrite(&cy, sizeof(int), 1, chunkfile);

    fseek(chunkfile, 0x10, SEEK_SET);

    fwrite(chunk -> overlay_tiles,      sizeof(uint), 256, chunkfile);
    fwrite(chunk -> foreground_tiles,   sizeof(uint), 256, chunkfile);

    fclose(chunkfile);
}
