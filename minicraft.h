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

#define RENDER_SCALE 4
#define RENDER_DISTANCE 3
#define RENDER_WINX 240
#define RENDER_WINY 160

#define WORLD_PERLIN_SCALE 0.05
#define WORLD_WATER_SCALE 0.25

#define TEXTURE_MULTIPLE 0x01
#define TEXTURE_SINGLE   0x02

#define TILE_BREAKABLE   0x01
#define TILE_TRANSPARENT 0x02
#define TILE_SOLID       0x03

#define ENTITY_MAX 0xFF

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
    double x, y;
}Coord2d;

//2D integer coordinate
typedef struct Coord2i{
    int x, y;
}Coord2i;

typedef struct Color{
    uchar r, g, b;
}Color;

typedef struct Video_Mode{
    Coord2i viewport;
    int scale;
}Video_Mode;

//TODO: This is pointless
//Camera struct for player entity
typedef struct Camera{
    Coord2d position;
} Camera;

//OpenGL texture id with dimensions and texture uv information
typedef struct Texture{
    GLuint id;          //OpenGL texture id
    uint width;         //Size of texture in pixels
    uint height;        //------------------------
    uint tile_size;     //Size of tiles
    Coord2d atlas_uvs;  //Difference in UV coordinates per texture
} Texture;

typedef struct Font{
    Texture* t;
    std::string font_atlas;
} Font;

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

typedef struct Storage{
    uchar item_v[PLAYER_INVENTORY_SIZE]{};
    uchar item_c[PLAYER_INVENTORY_SIZE]{};
} Storage;

//Simple AABB
typedef struct BoundingBox{
    Coord2d p1; //Upper Left coordinate
    Coord2d p2; //Upper Right coordinate
} BoundingBox;

//All Entity types must contain an Entity member 'e' as their first member
typedef struct Entity{
    uint id;            //id of the entity in g_entity_registry
    Coord2d position;   //position of the Entity
    BoundingBox bounds; //Bounding Box for entity
    EntityType type;    //Type enum of Entity. Def. ENT_GENERIC
}Entity;

//Player entity
typedef struct Entity_Player{
    Entity  e{e.type = ENT_PLAYER};     //Entity inheritance
    Camera  camera{{0, 0}};             //Player camera
    Storage inventory;
    uint    health = 10;
}Entity_Player;

//World chunk
typedef struct Chunk{
    Coord2i pos{0,0};               //Chunk coordinates
    uchar overlay_tiles[256]{};     //Base tiles
    uchar foreground_tiles[256]{};  //Overlay tiles
}Chunk;

typedef struct Time{
    double delta;   //The time past between the previous and current frames
    double global;  //Absolute time since game was started
} Time;


/*--- Global objects and registries. Globals are bad but so am I ---*/

//minicraft_time.cpp
extern Time* g_time; //Global time object

//minicraft_world.cpp
extern Chunk* g_chunk_buffer[];    //Contains loaded chunks
extern Block* g_block_registry[];  //Contains all Block types
extern Item*  g_item_registry[];   //Contains all Item types

//minicraft_input.cpp
extern bool g_k_keys[];   //Keyboard press booleans
extern bool g_m_keys[];   //Mouse button press booleans
extern int g_k_actions[]; //GLFW keyboard actions
extern int g_m_actions[]; //GLFW mouse button actions
extern Coord2d g_m_pos;   //Mouse position

//minicraft_entity.cpp
extern Entity*  g_entity_registry[]; //All active entities
extern uint     g_entity_highest_id; //The highest entity ID active in g_entity_registry

//minicraft_main.cpp
extern std::string g_game_path; //Global reference to argv[0]

//minicraft_rendering.cpp
extern Video_Mode g_video_mode;

/*--- Functions ---*/

Texture*    texture_generate(Image* img, uchar texture_load_options, uint tile_size);           //Generate Texture Object
Texture*    texture_load_bmp(const std::string&, uchar texture_load_options, uint tile_size);   //Load a 24-bit BMP
void        texture_bind(Texture* t, GLuint sampler);                                           //Bind texture to GL_TEXTURE_2D

void    world_unload_chunk(const std::string& savename, Chunk* chunk);                                  //Safely save and delete Chunk
Chunk*  world_load_chunk(const std::string& savename, Coord2i ccoord, int seed);                        //Load or generate Chunk
void    world_populate_chunk_buffer(const std::string& savename, Camera* cam);                          //Populate Chunk Buffer
Chunk*  world_get_chunk(Coord2i ccoord);                                                                //Find a chunk in g_chunk_buffer
void    world_modify_chunk(const std::string& savename, Coord2i ccoord, Coord2i tcoord, uint value);    //Set tile of chunk to value
Chunk*  world_chunkfile_read(const std::string& savename, Coord2i ccoord);                              //read chunk from file
void    world_chunkfile_write(const std::string& savename, Chunk* chunk);                               //write chunk to chunkfile

GLFWwindow* rendering_init_opengl(uint window_x, uint window_y, uint scale);                //Init OpenGL, GLFW, and create window
void        rendering_draw_chunk(Chunk* chunk, Texture* atlas_texture, Camera* camera);     //Draw a chunk
void        rendering_draw_entity(Entity* entity, Camera* camera);                          //Draw an entity
void        rendering_draw_chunk_buffer(Texture* atlas_texture, Camera* camera);            //Draw the chunk buffer
void        rendering_draw_text(const std::string& text, uint size, Font* font, Color color, Coord2i pos);
void        rendering_draw_hud(Entity_Player* player, Texture* ui_texture_sheet);
Coord2d     rendering_viewport_to_world_pos(Camera* cam, Coord2d pos);                      //Get world position of viewport position

void input_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);   //Keyboard callback
void input_mouse_button_callback(GLFWwindow* window, int button, int actions, int mods);    //Mouse button callback
void input_mouse_position_callback(GLFWwindow* window, double xpos, double ypos);           //Mouse position callback
void input_register_callbacks(GLFWwindow* window);                                          //Register all callbacks with GLFW

bool input_get_key(int keycode);        //Get key status
bool input_get_button(int keycode);     //Get button status
bool input_get_key_down(int keycode);   //Get new keystroke
bool input_get_mouse_down(int keycode); //Get new mouse stroke
void input_poll_input();                //Input poll

void time_update_time(double glfw_time);    //Update time
int  time_get_framerate();                  //Get FPS

Entity* entity_create(Entity* entity);                                          //Add entity to entity_registry and assign id. Returns pointer address for convenience
void    entity_move(Entity* entity, Coord2d delta, bool respect_collisions);    //Move an entity
void    entity_delete(uint id);                                                 //Removes entity to entity_registry and deletes Entity
void    entity_tick();                                                          //Ticks all entities

int storage_add_item(Storage* storage, uint item_id, uint item_count);


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
