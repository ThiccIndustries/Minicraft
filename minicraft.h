/*
 * Created by MajesticWaffle on 4/27/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#ifndef MINICRAFT_MINICRAFT_H
#define MINICRAFT_MINICRAFT_H

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "iostream"


/*--- Settings, bitflags, and selectors ---*/

#define RENDER_DISTANCE 3
#define RENDER_WINX 240
#define RENDER_WINY 160

#define WORLD_PERLIN_SCALE 0.05
#define WORLD_WATER_SCALE 0.25

#define TEXTURE_TILE_RES 16
#define TEXTURE_MULTIPLE 0x01
#define TEXTURE_SINGLE   0x02

#define TILE_BREAKABLE   0x01
#define TILE_TRANSPARENT 0x02
#define TILE_SOLID       0x03

#define ENTITY_MAX 0xFF
#define ENTITY_GENERIC 0
#define ENTITY_PLAYER  1

#define PLAYER_INVENTORY_SIZE       256
#define PLAYER_INVENTORY_STACK_SIZE 64


/*--- Enums and structs ---*/

enum Material{
    STONE,      //Requires pickaxe to break effectively
    WOOD,       //Requires axe to break effectively
    EARTH,      //Requires shovel to break effectively
    SOLID       //NO BUFF
};

typedef unsigned int  uint;
typedef unsigned char uchar;

typedef struct{
    double x, y;
}Coord2d;

typedef struct{
    int x, y;
}Coord2i;

typedef struct{
    Coord2d position;
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
    uint atlas_index;   //Index of texture
    uchar options;      //Tile bitflags
    Material material;  //Tile material type
    uint drop_id;       //ID of resource to drop
    uint drop_count;    //drop count
}Block;

typedef struct{
    uint atlas_index;
}Item;

//All Entity types must contain an Entity member 'e' as their first member
typedef struct{
    uint id;            //id of the entity in EntityRegistry
    Coord2i position;   //position of the Entity
}Entity;

typedef struct{
    Entity e;                               //Entity inheritance
    Camera* camera;                         //Player camera
    //TODO: Replace this with a universal storage system probably
    uint item_v[PLAYER_INVENTORY_SIZE];     //Item slot ids
    uint item_c[PLAYER_INVENTORY_SIZE];     //Item slot counts
}Entity_Player;

typedef struct{
    int pos_x;                      //Chunk coordinates
    int pos_y;                      //-----------------
    uint overlay_tiles[16 * 16];    //Base tiles
    uint foreground_tiles[16 * 16]; //Overlay tiles
}Chunk;

typedef struct{
    double delta;   //The time past between the previous and current frames
    double global;  //Absolute time since game was started
} Time;


/*--- Global objects and registries ---*/

    //Loc: minicraft_time.cpp
extern Time* mTime;

    //Loc: minicraft_world.cpp
extern Chunk* ChunkBuffer[];
extern Block* BlockRegistry[];
extern Item*  ItemRegistry[];

    //Loc: minicraft_input.cpp
extern bool k_keys[];
extern bool m_keys[];

extern int k_actions[];
extern int m_actions[];

extern double m_posx;
extern double m_posy;

    //Loc: minicraft_entity.cpp
extern Entity* entity_registry[];


/*--- Functions (prefix = file location)---*/

Texture*    texture_generate(Image* img, uchar texture_load_options);   //Generate Texture Object
Image*      texture_load_bmp(const char* path);                         //Load a 24-bit BMP
void        texture_bind(Texture* t, GLuint sampler);                   //Bind texture to GL_TEXTURE_2D

void    world_unload_chunk(Chunk* chunk);                                       //Safely save and delete Chunk
Chunk*  world_load_chunk(int x, int y, int seed);                               //Load or generate Chunk
void    world_populate_chunk_buffer(Camera* cam);                               //Populate Chunk Buffer
void    world_modify_chunk(int cx, int cy, int tx, int ty, int block_id);       //Set tile of chunk to value
Block*  world_construct_block(uint atlas_index, uchar options, Material mat,
                              uint drop_id, uint drop_count);                   //Build new Chunk Struct

GLFWwindow* rendering_init_opengl(uint window_x, uint window_y, uint scale);                        //Init OpenGL, GLFW, and create window
void        rendering_draw_chunk(Chunk* chunk, Texture* atlas_texture, Camera* camera);             //Draw a chunk
void        rendering_draw_chunk_buffer(Texture* atlas_texture, Camera* camera);                    //Draw the chunk buffer
Coord2d     rendering_viewport_to_world_pos(GLFWwindow* window, Camera* cam, double x, double y);   //Get world position of viewport position

void input_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);   //Keyboard callback
void input_mouse_button_callback(GLFWwindow* window, int button, int actions, int mods);    //Mouse button callback
void input_moues_position_callback(GLFWwindow* window, double xpos, double ypos);           //Mouse position callback
void input_register_callbacks(GLFWwindow* window);                                          //Register all callbacks with GLFW

bool input_get_key(int keycode);        //Get key status
bool input_get_button(int keycode);     //Get button status
bool input_get_key_down(int keycode);   //Get new keystroke
bool input_get_mouse_down(int keycode); //Get new mouse stroke
void input_poll_input();    //Input poll

void time_update_time(double glfw_time);    //Update time
int time_get_framerate();                   //Get FPS

void entity_register_entity(Entity* entity);    //Add entity to entity_registry and assign id

int player_inventory_add_item(Entity_Player* player, uint item_id, uint item_count);


/*---inline util functions---*/

inline int clampi(int a, int min, int max){
    if(a > max) return max;
    if(a < min) return min;
    return a;
}

#endif
