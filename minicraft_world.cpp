#include "minicraft.h"
#include "PerlinNoise.hpp"

Chunk* ChunkBuffer[RENDER_DISTANCE * RENDER_DISTANCE * 4];

void world_populate_chunk_buffer(Camera* cam){
    int player_chunk_x = cam -> pos_x / 256;
    int player_chunk_y = cam -> pos_y / 256;

    for(int x = 0; x < RENDER_DISTANCE * 2; x++){
        for(int y = 0; y < RENDER_DISTANCE * 2; y++){

            int chunk_x = player_chunk_x - (x - RENDER_DISTANCE);
            int chunk_y = player_chunk_y - (y - RENDER_DISTANCE);
            int chunki = x + (y * RENDER_DISTANCE * 2);

            if(ChunkBuffer[chunki] == nullptr || ChunkBuffer[chunki] -> pos_x != chunk_x || ChunkBuffer[chunki] -> pos_x != chunk_y){
                world_unload_chunk(ChunkBuffer[chunki]);
                ChunkBuffer[chunki] = world_load_chunk(chunk_x, chunk_y, 0);
            }
        }
    }
}

Chunk* world_load_chunk(int x, int y, int seed){

    siv::PerlinNoise pn(seed);

    Chunk* chunkptr = new Chunk;

    double perlin_coord_y1, perlin_coord_x, perlin_coord_y;

    for(int tx = 0; tx < 16; tx++){
        for(int ty = 0; ty < 16; ty++){
            perlin_coord_x = ((x * 256) + tx) * WORLD_PERLIN_SCALE;

            perlin_coord_y = ((x * 256) + ty) * WORLD_PERLIN_SCALE;
            perlin_coord_y1 = ((x * 256) + (ty - 1)) * WORLD_PERLIN_SCALE;

            double chunk_value = pn.noise2D(perlin_coord_x, perlin_coord_y);
            double chunk_value1 = pn.noise2D(perlin_coord_x, perlin_coord_y1);

            if(chunk_value >= WORLD_WATER_SCALE){
                if(chunk_value1 >= WORLD_WATER_SCALE)
                    chunkptr -> foreground_tiles[(ty * 16) + tx] = 4;
                else
                    chunkptr -> foreground_tiles[(ty * 16) + tx] = 5;
            }else
                chunkptr -> foreground_tiles[(ty * 16) + tx] = 0;

        }
    }

    chunkptr -> pos_x = x;
    chunkptr -> pos_y = y;
    return chunkptr;
}

void world_unload_chunk(Chunk* chunk){
    //TODO: save chunks to file
    delete chunk;
}