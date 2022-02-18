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

    Block* tile_properties = new Block[]{
        /*        Tile Definitions
                  Texture   Options                         DropID  DropCount */

        //Background Tiles
        {0,        0,                                  0,      0}, //Grass
        {1,        TILE_SOLID,                         0,      0}, //Tree_Base
        {2,        TILE_SOLID,                         0,      0}, //Tree_Top

        {6,        TILE_SOLID,                         0,      0}, //Water

        {3,        TILE_SOLID,                         0,      0}, //Water_Corner_UL
        {3,        TILE_SOLID | TILE_TEX_FLIP_X,       0,      0}, //Water_Corner_UR

        {4,        TILE_SOLID,                         0,      0}, //Water_Top

        {5,        TILE_SOLID,                         0,      0}, //Water_ICorner_UL
        {5,        TILE_SOLID | TILE_TEX_FLIP_X,       0,      0}, //Water_ICorner_UR

        {11,       TILE_SOLID,                         0,     0}, //Water_Side_L
        {11,       TILE_SOLID | TILE_TEX_FLIP_X,       0,     0}, //Water_Side_R

        {19,       TILE_SOLID,                         0,     0}, //Water_Corner_DL
        {19,       TILE_SOLID | TILE_TEX_FLIP_X,       0,     0}, //Water_Corner_DR

        {12,       TILE_SOLID,                         0,     0}, //Water_Bottom

        {13,       TILE_SOLID,                         0,     0}, //Water_ICorner_DL
        {13,       TILE_SOLID | TILE_TEX_FLIP_X,       0,     0}, //Water_ICorner_DR

        {14,       TILE_SOLID,                         0,      0}, //Water_U_Top
        {15,       TILE_SOLID,                         0,      0}, //Water_U_Bottom
        {20,       TILE_SOLID,                         0,      0}, //Water_U_L
        {20,       TILE_SOLID | TILE_TEX_FLIP_X,       0,      0}, //Water_U_R

        {22,       TILE_SOLID,                         0,      0}, //Water_U_Corner_Top
        {7,        TILE_SOLID,                         0,      0}, //Water_U_Corner_Bottom
        {21,       TILE_SOLID,                         0,      0}, //Water_U_Corner_L
        {21,       TILE_SOLID | TILE_TEX_FLIP_X,       0,      0}, //Water_U_Corner_R

        //Collectables
        {8,        TILE_COLLECTABLE,                   1,      1}, //Flower
        {9,        TILE_COLLECTABLE,                   2,      1}, //Lily
        {10,       TILE_SOLID | TILE_COLLECTABLE,      3,      1}, //Wood
        {16,       TILE_COLLECTABLE | TILE_ANIMATED,   4,      1}, //Dead_Zombie
        {17,       TILE_COLLECTABLE,                   5,      1}, //Bone fragments
};


Chunk* world_load_chunk(Map* map, Coord2i coord){
    Chunk* chunkptr = world_chunkfile_read(map, coord );

    //Chunk successfully loaded from file
    if(chunkptr != nullptr) return chunkptr;

    chunkptr = new Chunk;
    //Generate chunk
    siv::PerlinNoise noise;
    noise.reseed(g_save -> s.seed);
    for (int tx = 0; tx < 16; ++tx) {
        for (int ty = 0; ty < 16; ++ty) {
            bool water[9] = {false};

            for(int wx = -1; wx <= 1; ++wx){
                for(int wy = -1; wy <= 1; ++wy){
                    water[((wy + 1) * 3) + (wx + 1)] = noise.noise2D((float) coord.x + ((float) (tx + wx) / 16), (float) coord.y + ((float) (ty + wy) / 16)) >= 0.1;
                }
            }
            uint tile = GRASS;

            if (water[4])
                tile = WATER;


            //TODO: This is the worst thing ever wow.
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
    world_chunkfile_write(map, chunkptr);

    return chunkptr;
}

void world_unload_chunk(Map* map, Chunk* chunk){
    world_chunkfile_write(map, chunk);
}

void  world_save_game(Save* save){
    write_map_resource(save -> overworld, "save.dat", &(save -> s), sizeof(Save_Data));
}


Save* world_load_game(uint id){
    Save* save = new Save;
    Map* map = world_map_read(id);

    if(map == nullptr){
        Texture* terr = texture_load_bmp(get_resource_path(g_game_path, "resources/terrain.bmp"), TEXTURE_MULTIPLE | TEXTURE_STORE, 16);

        Map* map1 = new Map;
        map1 -> id = id;
        map1 -> tilemap = terr;
        map1 -> tile_count = 29;
        std::cout << sizeof(*tile_properties) << " : " << sizeof(Block) << std::endl;
        map1 -> tile_properties = tile_properties;
        world_map_write(map1);

        //Copy resources
        std::filesystem::copy_file(get_resource_path(g_game_path, "resources/terrain.bmp").c_str(), get_resource_path(g_game_path, "maps/" + std::to_string(map1 -> id) + ".map/texture.bmp").c_str());
        map = map1;
    }

    Save_Data s;
    s.player_position = {0, 0};
    s.seed = std::time(0);

    read_map_resource(map, "save.dat", &s, sizeof(Save_Data));

    save -> s = s;
    save -> overworld = map;
    return save;
}
