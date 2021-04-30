/*
 * Created by MajesticWaffle on 4/26/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

VideoMode g_video_mode;

GLFWwindow* rendering_init_opengl(uint window_x, uint window_y, uint scale){

    //Init GLFW
    if(glfwInit() != GLFW_TRUE){
        return nullptr;
    }

    //Create window and set context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* windowptr = glfwCreateWindow(window_x * scale, window_y * scale, "Minicraft", nullptr, nullptr);
    glfwMakeContextCurrent(windowptr);
    glewInit();

    //Set up ortho projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window_x, window_y, 0, 1, -1);
    glMatrixMode(GL_PROJECTION); //TODO: why is this here twice.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    g_video_mode.viewport    = {(int)window_x, (int)window_y};
    g_video_mode.scale       = scale;

    return windowptr;
}

void rendering_draw_chunk(Chunk* chunk, Texture* atlas_texture, Camera* camera){

    double chunk_x = chunk -> pos_x * (16 * 16) - (camera -> position.x - (g_video_mode.viewport.x / 2));
    double chunk_y = chunk -> pos_y * (16 * 16) - (camera -> position.y - (g_video_mode.viewport.y / 2));

    for(int y = 0; y < 16; y++){
        for(int x = 0; x < 16; x++){

            if(chunk_x + (x * 16) > RENDER_WINX || chunk_x + (x * 16) < -16)
                continue;
            if(chunk_y + (y * 16) > RENDER_WINY || chunk_y + (y * 16) < -16)
                continue;

            uint texture_coord_x = g_block_registry[chunk -> foreground_tiles[(y * 16) + x]] -> atlas_index % (atlas_texture -> width / TEXTURE_TILE_RES);
            uint texture_coord_y = g_block_registry[chunk -> foreground_tiles[(y * 16) + x]] -> atlas_index / (atlas_texture -> width / TEXTURE_TILE_RES);

            double texture_uv_x = atlas_texture -> atlas_uv_dx * texture_coord_x;
            double texture_uv_y = atlas_texture -> atlas_uv_dy * texture_coord_y;

            texture_bind(atlas_texture, 0);
            glEnable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);{
                glTexCoord2d(texture_uv_x,                                  texture_uv_y);                                  glVertex2d(chunk_x + (x * 16),        chunk_y + (y * 16));
                glTexCoord2d(texture_uv_x + atlas_texture -> atlas_uv_dx,   texture_uv_y);                                  glVertex2d(chunk_x + (x * 16) + 16,   chunk_y + (y * 16));
                glTexCoord2d(texture_uv_x + atlas_texture -> atlas_uv_dx,   texture_uv_y + atlas_texture -> atlas_uv_dy);   glVertex2d(chunk_x + (x * 16) + 16,   chunk_y + (y * 16) + 16);
                glTexCoord2d(texture_uv_x,                                  texture_uv_y + atlas_texture -> atlas_uv_dy);   glVertex2d(chunk_x + (x * 16),        chunk_y + (y * 16) + 16);
            }
            glEnd();
            glDisable(GL_TEXTURE_2D);

        }
    }
}

void rendering_draw_entity(Entity* entity, Camera* camera){
    double entity_x = entity -> position.x - (camera -> position.x - (g_video_mode.viewport.x / 2));
    double entity_y = entity -> position.y - (camera -> position.y - (g_video_mode.viewport.y / 2));

    //texture_bind(atlas_texture, 0);
    //glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);{
        glVertex2d(entity_x,        entity_y);
        glVertex2d(entity_x + 16,   entity_y);
        glVertex2d(entity_x + 16,   entity_y + 16);
        glVertex2d(entity_x,        entity_y + 16);
    }
    glEnd();
    //glDisable(GL_TEXTURE_2D);
}

void rendering_draw_chunk_buffer(Texture* atlas_texture, Camera* camera){
    for(int i = 0; i < RENDER_DISTANCE * RENDER_DISTANCE * 4; ++i){
        rendering_draw_chunk(g_chunk_buffer[i], atlas_texture, camera);
    }
}

Coord2d rendering_viewport_to_world_pos(Camera* cam, Coord2d coord){
    Coord2d position;

    position.x = (coord.x / g_video_mode.scale) + (cam -> position.x - (g_video_mode.viewport.x / 2));
    position.y = (coord.y / g_video_mode.scale) + (cam -> position.y - (g_video_mode.viewport.y / 2));

    return position;
}