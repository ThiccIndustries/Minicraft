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
    MAT_STONE,      //Requires pickaxe to break effectively
    MAT_WOOD,       //Requires axe to break effectively
    MAT_EARTH,      //Requires shovel to break effectively
    MAT_SOLID       //NO BUFF
};

enum EntityType{
    ENT_GENERIC,
    ENT_PLAYER
};
typedef unsigned int  uint; //Shorthand unsigned int type
typedef unsigned char uchar; //Shorthand unsigned char type

//2D double coordinate
typedef struct Coord2d{
    double x = 0, y = 0;
}Coord2d;

//2D integer coordinate
typedef struct Coord2i{
    int x = 0, y = 0;
}Coord2i;

//TODO: This seems pointless, is it gonna have anything added?
//Camera struct for player entity
typedef struct Camera{
    Coord2d position;
} Camera;

//OpenGL texture id with dimensions and texture uv information
typedef struct Texture{
    GLuint id;          //OpenGL texture id
    uint width;         //Size of texture in pixels
    uint height;        //------------------------
    double atlas_uv_dx; //Difference in UV coordinates per texture
    double atlas_uv_dy; //---------------------------------------
} Texture;

//Raw image data loaded from a BMP image
typedef struct Image{
    uint width;     //Size of image in pixels
    uint height;    //----------------------
  uchar* imageData; //Raw image data in RGB bmp format
} Image;

//World tile
typedef struct Block{
    uint atlas_index;   //Index of texture
    uchar options;      //Tile bitflags
    Material material;  //Tile material type
    uint drop_id;       //ID of resource to drop
    uint drop_count;    //drop count
}Block;

//Inventory item
typedef struct Item{
    uint atlas_index;
}Item;

//All Entity types must contain an Entity member 'e' as their first member
typedef struct Entity{
    uint id;            //id of the entity in EntityRegistry
    Coord2i position;   //position of the Entity
    EntityType type;
}Entity;

//Player entity
typedef struct Entity_Player{
    Entity e{e.type = ENT_PLAYER};          //Entity inheritance
    Camera camera;                          //Player camera
    //TODO: Replace this with a universal storage system probably
    uint item_v[PLAYER_INVENTORY_SIZE]{};   //Item slot ids
    uint item_c[PLAYER_INVENTORY_SIZE]{};   //Item slot counts

}Entity_Player;

//World chunk
typedef struct Chunk{
    int pos_x = 0;                  //Chunk coordinates
    int pos_y = 0;                  //-----------------
    uint overlay_tiles[256]{};      //Base tiles
    uint foreground_tiles[256]{};   //Overlay tiles
}Chunk;

typedef struct Time{
    double delta;   //The time past between the previous and current frames
    double global;  //Absolute time since game was started
} Time;


/*--- Global objects and registries ---*/

    //Loc: minicraft_time.cpp

extern Time* mTime; //Global time object

    //Loc: minicraft_world.cpp

extern Chunk* ChunkBuffer[];    //Contains chunks currently in render distance
extern Block* BlockRegistry[];  //Contains all Block types
extern Item*  ItemRegistry[];   //Contains all Item types

    //Loc: minicraft_input.cpp

extern bool k_keys[];   //Keyboard press booleans
extern bool m_keys[];   //Mouse button press booleans

extern int k_actions[]; //GLFW keyboard actions
extern int m_actions[]; //GLFW mouse button actions

extern double m_posx;   //Mouse position in pixels
extern double m_posy;   //------------------------

    //Loc: minicraft_entity.cpp

extern Entity* EntityRegistry[]; //All active entities

    //Loc: minicraft_main.cpp

extern std::string game_root_path;

/*--- Functions (prefix = file location)---*/

Texture*    texture_generate(Image* img, uchar texture_load_options);   //Generate Texture Object
Image*      texture_load_bmp(const char* path);                         //Load a 24-bit BMP
void        texture_bind(Texture* t, GLuint sampler);                   //Bind texture to GL_TEXTURE_2D

void    world_unload_chunk(std::string savename, Chunk* chunk);                             //Safely save and delete Chunk
Chunk*  world_load_chunk(std::string savename, int x, int y, int seed);                     //Load or generate Chunk
void    world_populate_chunk_buffer(std::string savename, Camera* cam);                     //Populate Chunk Buffer
void    world_modify_chunk(std::string savename, int cx, int cy, int tx, int ty, int value);//Set tile of chunk to value
Block*  world_construct_block(uint atlas_index, uchar options, Material mat,
                              uint drop_id, uint drop_count);                               //Build new Chunk Struct
Chunk*  world_chunkfile_read(std::string savename, uint cx, uint cy);                       //read chunk from file
void    world_chunkfile_write(std::string savename, Chunk* chunk);                          //write chunk to chunkfile

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
int  time_get_framerate();                  //Get FPS

Entity* entity_create(Entity* entity);      //Add entity to entity_registry and assign id. Returns pointer address for convenience
void    entity_delete(uint id);             //Removes entity to entity_registry and deletes Entity

int player_inventory_add_item(Entity_Player* player, uint item_id, uint item_count);


/*---inline util functions---*/

inline int clampi(int a, int min, int max){
    if(a > max) return max;
    if(a < min) return min;
    return a;
}

inline std::string get_resource_path(const std::string& executable_path, const std::string& resource_name){
    uint substri = executable_path.find_last_of('/');
    return executable_path.substr(0, substri) + "/" + resource_name;
}

#endif
