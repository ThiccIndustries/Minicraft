/*
 * Created by MajesticWaffle on 7/13/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */

#include <ctime>
#include <filesystem>
#include "minicraft.h"
#include "PerlinNoise.hpp"

//Mat type
enum Material{
    MAT_BACK,       //Background tiles
    MAT_DROP,       //Collectables
    MAT_BLCK,       //Block entities
};

enum{
    GRASS,
    TREE_BASE,
    TREE_TOP,
    WATER,
    WATER_CORNER_UL,
    WATER_CORNER_UR,
    WATER_TOP,
    WATER_ICORNER_UL,
    WATER_ICORNER_UR,
    WATER_SIDE_L,
    WATER_SIDE_R,
    WATER_CORNER_DL,
    WATER_CORNER_DR,
    WATER_BOTTOM,
    WATER_ICORNER_DL,
    WATER_ICORNER_DR,
    WATER_U_TOP,
    WATER_U_BOTTOM,
    WATER_U_L,
    WATER_U_R,
    WATER_UCORNER_TOP,
    WATER_UCORNER_BOTTOM,
    WATER_UCORNER_L,
    WATER_UCORNER_R,
    FLOWER,
    LILY,
    WOOD
};

Block* g_block_registry[255] = {
        /*        Tile Definitions
                  Texture   Options                         DropID  DropCount */

        //Background Tiles
        new Block{0,        0,                              0,      0}, //Grass
        new Block{1,        TILE_SOLID,                     0,      0}, //Tree_Base
        new Block{2,        TILE_SOLID,                     0,      0}, //Tree_Top

        new Block{6,        TILE_SOLID,                     0,      0}, //Water

        new Block{3,        TILE_SOLID,                     0,      0}, //Water_Corner_UL
        new Block{3,        TILE_SOLID | TILE_TEX_FLIP_X,   0,      0}, //Water_Corner_UR

        new Block{4,        TILE_SOLID,                     0,      0}, //Water_Top

        new Block{5,        TILE_SOLID,                     0,      0}, //Water_ICorner_UL
        new Block{5,        TILE_SOLID | TILE_TEX_FLIP_X,   0,      0}, //Water_ICorner_UR

        new Block{11,       TILE_SOLID,                     0,     0}, //Water_Side_L
        new Block{11,       TILE_SOLID | TILE_TEX_FLIP_X,   0,     0}, //Water_Side_R

        new Block{19,       TILE_SOLID,                     0,     0}, //Water_Corner_DL
        new Block{19,       TILE_SOLID | TILE_TEX_FLIP_X,   0,     0}, //Water_Corner_DR

        new Block{12,       TILE_SOLID,                     0,     0}, //Water_Bottom

        new Block{13,       TILE_SOLID,                     0,     0}, //Water_ICorner_DL
        new Block{13,       TILE_SOLID | TILE_TEX_FLIP_X,   0,     0}, //Water_ICorner_DR

        new Block{14,       TILE_SOLID,                     0,      0}, //Water_U_Top
        new Block{15,       TILE_SOLID,                     0,      0}, //Water_U_Bottom
        new Block{20,       TILE_SOLID,                     0,      0}, //Water_U_L
        new Block{20,       TILE_SOLID | TILE_TEX_FLIP_X,   0,      0}, //Water_U_R

        new Block{22,       TILE_SOLID,                     0,      0}, //Water_U_Corner_Top
        new Block{7,        TILE_SOLID,                     0,      0}, //Water_U_Corner_Bottom
        new Block{21,       TILE_SOLID,                     0,      0}, //Water_U_Corner_L
        new Block{21,       TILE_SOLID | TILE_TEX_FLIP_X,   0,      0}, //Water_U_Corner_R

        //Collectables
        new Block{8,        TILE_COLLECTABLE,               1,      1}, //Flower
        new Block{9,        TILE_COLLECTABLE,               2,      1}, //Lily
        new Block{10,       TILE_SOLID | TILE_COLLECTABLE,  3,      1}, //Wood
};


Chunk* world_load_chunk(Coord2i coord){
    Chunk* chunkptr = world_chunkfile_read("saves/" + g_save -> world_name + "/chunks", coord );

    //Chunk successfully loaded from file
    if(chunkptr != nullptr) return chunkptr;

    chunkptr = new Chunk;

    //Generate chunk
    siv::PerlinNoise noise;
    noise.reseed(g_save -> seed);
    for (int tx = 0; tx < 16; ++tx) {
        for (int ty = 0; ty < 16; ++ty) {
            bool water[9] = {false};

            for(int wx = -1; wx <= 1; ++wx){
                for(int wy = -1; wy <= 1; ++wy){
                    water[((wy + 1) * 3) + (wx + 1)] = noise.noise2D((float) coord.x + ((float) (tx + wx) / 16), (float) coord.y + ((float) (ty + wy) / 16)) >= 0.1;
                }
            }
            uint tile = 0;

            if (water[4])
                tile = WATER;


            //TODO: Yanderedev-ass code holy shit
            //Pass 1
            if (tile == WATER && !water[1])
                tile = WATER_TOP;

            if (tile == WATER && !water[7])
                tile = WATER_BOTTOM;

            if (tile == WATER && !water[3])
                tile = WATER_SIDE_L;

            if (tile == WATER && !water[5])
                tile = WATER_SIDE_R;

            //Pass 2
            if (tile == WATER_TOP && !water[3] && water[5] && water[7])
                tile = WATER_CORNER_UL;

            if (tile == WATER_TOP && !water[5] && water[3] && water[7])
                tile = WATER_CORNER_UR;

            if (tile == WATER_BOTTOM && !water[3] && water[5] && water[1])
                tile = WATER_CORNER_DL;

            if (tile == WATER_BOTTOM && !water[5] && water[3] && water[1])
                tile = WATER_CORNER_DR;

            if (tile == WATER_TOP && !water[3] && !water[5])
                tile = WATER_U_TOP;

            if (tile == WATER_BOTTOM && !water[3] && !water[5])
                tile = WATER_U_BOTTOM;

            if (tile == WATER_TOP && !water[1] && !water[7] && water[5])
                tile = WATER_U_L;

            if (tile == WATER_TOP && !water[1] && !water[7] && water[3])
                tile = WATER_U_R;

            if (tile == WATER && !water[0] && water[2] && water[6])
                tile = WATER_ICORNER_UL;

            if (tile == WATER && !water[2] && water[0] && water[8])
                tile = WATER_ICORNER_UR;

            if (tile == WATER && !water[6] && water[8] && water[0])
                tile = WATER_ICORNER_DL;

            if (tile == WATER && !water[8] && water[6] && water[2])
                tile = WATER_ICORNER_DR;

            if (tile == WATER && !water[0] && !water[2])
                tile = WATER_UCORNER_TOP;

            if (tile == WATER && !water[6] && !water[8])
                tile = WATER_UCORNER_BOTTOM;

            if (tile == WATER && water[3] && !water[6] && !water[0])
                tile = WATER_UCORNER_L;

            if (tile == WATER && water[5] && !water[2] && !water[8])
                tile = WATER_UCORNER_R;

            chunkptr -> background_tiles[(ty * 16) + tx] = tile;

            if((int) ( ((double)rand() / RAND_MAX) * 100 ) == 1  ) {
                if(tile == WATER)
                    chunkptr -> overlay_tiles[(ty * 16) + tx] = LILY;
                if(tile == GRASS)
                    chunkptr -> overlay_tiles[(ty * 16) + tx] = FLOWER;
            }

        }
    }




    chunkptr -> pos = coord;
    world_chunkfile_write("saves/" + g_save -> world_name + "/chunks", chunkptr);

    return chunkptr;
}

void world_unload_chunk(Chunk* chunk){
    world_chunkfile_write("saves/" + g_save -> world_name + "/chunks", chunk);
    texture_destroy(chunk -> render_texture);
    delete[] chunk;
}

void  world_save_game(Save* save){
    int px = save -> player_position.x;
    int py = save -> player_position.y;

    std::string path = "saves/" + save -> world_name;

    std::filesystem::create_directories(get_resource_path(g_game_path, path));
    FILE* savefile = fopen(get_resource_path(g_game_path, path + "/meta.sf").c_str(), "wb");

    fwrite(&px, sizeof(int), 1, savefile);
    fwrite(&py, sizeof(int), 1, savefile);

    fseek(savefile, 0x10, SEEK_SET);
    fwrite(&save -> seed, sizeof(long), 1, savefile);

    fclose(savefile);
}


Save* world_load_game(const std::string& world_name){
    Save* save = new Save;
    FILE* savefile = fopen(get_resource_path(g_game_path, "saves/" + world_name + "/meta.sf").c_str(), "rb");
    save -> world_name = world_name;
    //No save found
    if(!savefile){
        save -> player_position = {0,0};
        save -> seed = std::time(0);
        world_save_game(save);
        return save;
    }

    fread(&save -> player_position.x, sizeof(int), 1, savefile);
    fread(&save -> player_position.y, sizeof(int), 1, savefile);

    fseek(savefile, 0x10, SEEK_SET);
    fread(&save -> seed, 1, sizeof(long), savefile);

    fclose(savefile);
    return save;
}
