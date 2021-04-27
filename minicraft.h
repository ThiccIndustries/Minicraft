//
// Created by Thicc Industries on 4/26/21.
//

#ifndef MINICRAFT_MINICRAFT_H
#define MINICRAFT_MINICRAFT_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "iostream"

#define RENDER_DISTANCE 4

#define WORLD_PERLIN_SCALE 0.05
#define WORLD_WATER_SCALE 0.25

#define TEXTURE_TILE_RES 16
#define TEXTURE_MULTIPLE 0x01
#define TEXTURE_SINGLE   0x02

#define TILE_BREAKABLE   0x01
#define TILE_TRANSPARENT 0x02
#define TILE_SOLID       0x03

//Mining speed buff if Tile material equals tool material
enum Material{
    STONE,      //Requires pickaxe to break effectively
    WOOD,       //Requires axe to break effectively
    EARTH       //Requires shovel to break effectively
};

typedef unsigned int  uint;
typedef unsigned char uchar;

typedef struct{
    int pos_x;  //Camera position
    int pos_y;  //--------------
} Camera;

typedef struct{
    GLuint id;          // OpenGL texture id
    uint width;         // Size of texture in pixels
    uint height;        // ------------------------
    double atlas_uv_dx; // Difference in UV coordinates per texture
    double atlas_uv_dy; // ---------------------------------------
} Texture;

typedef struct{
    uint width;     //Size of image in pixels
    uint height;    //----------------------
  uchar* imageData; //Raw image data in RGB bmp format
} Image;

typedef struct{
    uchar options;      //Tile bitflags
    Material material;  //Tile material type
    uint drop_id;       //ID of resource to drop
    uint drop_count;    //drop count
}Tile;

extern Tile tile_registry[256]; //TODO: this

typedef struct{
    int pos_x;
    int pos_y;
    uint overlay_tiles[16 * 16];
    uint foreground_tiles[16 * 16];
}Chunk;

extern Chunk* ChunkBuffer[];

Texture*    texture_generate(Image* img, uchar texture_load_options);   //Generate Texture Object
Image*      texture_load_bmp(const char* path);                         //Load a 24-bit BMP
void        texture_bind(Texture* t, GLuint sampler);                   //Bind texture to GL_TEXTURE_2D

void world_unload_chunk(Chunk* chunk);              //Safely save and delete Chunk
Chunk* world_load_chunk(int x, int y, int seed);    //Load or generate Chunk
void world_populate_chunk_buffer(Camera* cam);      //Populate Chunk Buffer

GLFWwindow* rendering_init_opengl(uint window_x, uint window_y, uint scale);
void rendering_draw_chunk(Chunk* chunk, Texture* atlas_texture, Camera* camera);

#endif
