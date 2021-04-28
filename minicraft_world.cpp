#include "minicraft.h"

Chunk* ChunkBuffer[RENDER_DISTANCE * RENDER_DISTANCE * 4];

Block* BlockRegistry[255] = {
        world_construct_block(0, 0, EARTH, 0, 0),   //Grass
        world_construct_block(2, 0, STONE, 0, 0),   //Stone
        world_construct_block(8, 0, EARTH, 0, 0),   //Sand

        world_construct_block(4, 0, SOLID, 0, 0),   //Water
        world_construct_block(5, 0, SOLID, 0, 0),   //Water Grass
        world_construct_block(6, 0, SOLID, 0, 0),  //Water Stone
        world_construct_block(7, 0, SOLID, 0, 0),   //Water Sand
};

void world_populate_chunk_buffer(Camera* cam){
    int player_chunk_x = cam -> pos_x / 256;
    int player_chunk_y = cam -> pos_y / 256;

    for(int x = 0; x < RENDER_DISTANCE * 2; x++){
        for(int y = 0; y < RENDER_DISTANCE * 2; y++){

            int chunk_x = player_chunk_x - (x - RENDER_DISTANCE);
            int chunk_y = player_chunk_y - (y - RENDER_DISTANCE);
            int chunki = x + (y * RENDER_DISTANCE * 2);

            //if(ChunkBuffer[chunki] == nullptr || ChunkBuffer[chunki] -> pos_x != chunk_x || ChunkBuffer[chunki] -> pos_x != chunk_y){
                world_unload_chunk(ChunkBuffer[chunki]);
                ChunkBuffer[chunki] = world_load_chunk(chunk_x, chunk_y, 0);
            //}
        }
    }
}

Chunk* world_load_chunk(int x, int y, int seed){
    Chunk* chunkptr = new Chunk;

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

void world_unload_chunk(Chunk* chunk){
    //TODO: save chunks to file
    delete chunk;
}

void world_modify_chunk(int cx, int cy, int tx, int ty, int value){
    //Look for chunk in chunk buffer
    Chunk* chunkptr;
    std::cout << "chunk:" << cx << " " << cy << std::endl;
    for(Chunk* c : ChunkBuffer){
        if(c -> pos_x == cx && c -> pos_y == cy){
            chunkptr = c;
            break;
        }
    }

    //Chunk isn't loaded
    if(chunkptr == nullptr){ return; }

    chunkptr -> foreground_tiles[tx + (ty * 16)] = value;
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