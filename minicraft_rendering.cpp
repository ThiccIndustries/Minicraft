/*
 * Created by MajesticWaffle on 4/26/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

Video_Mode g_video_mode;

//Some stuff I dont wanna deal with outside minicraft_rendering.cpp.
//TODO: put these back in minicraft.h? their own header?
void rendering_draw_healthbar(uint health, Texture* ui_texture, uint atlas_index);
void rendering_draw_cursor(Texture* ui_texture, uint atlas_index);

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
    glLoadIdentity();
    glOrtho(0, window_x, window_y, 0, 1, -1);
    glMatrixMode(GL_PROJECTION); //TODO: why is this here twice.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Mouse mode
    glfwSetInputMode(windowptr, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    g_video_mode.viewport    = {(int)window_x, (int)window_y};
    g_video_mode.scale       = scale;

    return windowptr;
}

void rendering_draw_chunk(Chunk* chunk, Texture* atlas_texture, Camera* camera){

    double chunk_x = chunk -> pos.x * (16 * 16) - (camera -> position.x - (g_video_mode.viewport.x / 2));
    double chunk_y = chunk -> pos.y * (16 * 16) - (camera -> position.y - (g_video_mode.viewport.y / 2));

    for(int y = 0; y < 16; y++){
        for(int x = 0; x < 16; x++){

            if(chunk_x + (x * 16) > RENDER_WINX || chunk_x + (x * 16) < -16)
                continue;
            if(chunk_y + (y * 16) > RENDER_WINY || chunk_y + (y * 16) < -16)
                continue;

            uint texture_coord_x = g_block_registry[chunk -> foreground_tiles[(y * 16) + x]] -> atlas_index % (atlas_texture -> width / atlas_texture -> tile_size);
            uint texture_coord_y = g_block_registry[chunk -> foreground_tiles[(y * 16) + x]] -> atlas_index / (atlas_texture -> width / atlas_texture -> tile_size);

            double texture_uv_x = atlas_texture -> atlas_uvs.x * texture_coord_x;
            double texture_uv_y = atlas_texture -> atlas_uvs.y * texture_coord_y;

            texture_bind(atlas_texture, 0);
            glEnable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);{
                glTexCoord2d(texture_uv_x,                                  texture_uv_y);                                  glVertex2d(chunk_x + (x * 16),        chunk_y + (y * 16));
                glTexCoord2d(texture_uv_x + atlas_texture -> atlas_uvs.x,   texture_uv_y);                                  glVertex2d(chunk_x + (x * 16) + 16,   chunk_y + (y * 16));
                glTexCoord2d(texture_uv_x + atlas_texture -> atlas_uvs.x,   texture_uv_y + atlas_texture -> atlas_uvs.y);   glVertex2d(chunk_x + (x * 16) + 16,   chunk_y + (y * 16) + 16);
                glTexCoord2d(texture_uv_x,                                  texture_uv_y + atlas_texture -> atlas_uvs.y);   glVertex2d(chunk_x + (x * 16),        chunk_y + (y * 16) + 16);
            }
            glEnd();
            glDisable(GL_TEXTURE_2D);

        }
    }
}

void rendering_draw_entity(Entity* entity, Camera* camera){
    double entity_x = entity -> position.x - (camera -> position.x - (g_video_mode.viewport.x / 2));
    double entity_y = entity -> position.y - (camera -> position.y - (g_video_mode.viewport.y / 2));

    //TODO: sprites
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

void rendering_draw_text(const std::string& text, uint size, Font* font, Color color, Coord2i pos){
    char c;         //Char being drawn
    uint ci;        //The index of the char in font -> font_atlas
    uint    ci_x,   ci_y;  //The 2D position of the char in font -> t
    double  uv_x,   uv_y;
    int     pos_x,  pos_y;
    int     tilesize = font -> t -> tile_size * size;
    pos_y = pos.y;

    //No point in doing this over and over
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glColor3ub(color.r, color.g, color.b);
    texture_bind(font -> t, 0);

    for(int i = 0; i < text.length(); ++i){
        c = text.at(i);

        ci = font -> font_atlas.find(c);
        ci_y = ci / 10;
        ci_x = ci % 10;

        uv_x = font -> t -> atlas_uvs.x * ci_x;
        uv_y = font -> t -> atlas_uvs.y * ci_y;

        pos_x = pos.x + (i * tilesize);

        glBegin(GL_QUADS);{
            glTexCoord2d(uv_x                           , uv_y                              ); glVertex2i(pos_x           , pos_y);
            glTexCoord2d(uv_x + font -> t -> atlas_uvs.x, uv_y                              ); glVertex2i(pos_x + tilesize, pos_y);
            glTexCoord2d(uv_x + font -> t -> atlas_uvs.x, uv_y + font -> t -> atlas_uvs.y   ); glVertex2i(pos_x + tilesize, pos_y + tilesize);
            glTexCoord2d(uv_x                           , uv_y + font -> t -> atlas_uvs.y   ); glVertex2i(pos_x           , pos_y + tilesize);
        }
        glEnd();

    }

    glColor3ub(255, 255, 255);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void rendering_draw_cursor(Texture* ui_texture, uint atlas_index){
    double x, y;
    double uv_x, uv_y;

    x = g_m_pos.x / g_video_mode.scale;
    y = g_m_pos.y / g_video_mode.scale;

    uv_x = (atlas_index % (ui_texture -> width / ui_texture -> tile_size)) * ui_texture -> atlas_uvs.x;
    uv_y = (atlas_index / (ui_texture -> height / ui_texture -> tile_size)) * ui_texture -> atlas_uvs.y;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    texture_bind(ui_texture, 0);

    glBegin(GL_QUADS);{
        glTexCoord2d(uv_x                            , uv_y                             ); glVertex2d(x                             , y);
        glTexCoord2d(uv_x + ui_texture -> atlas_uvs.x, uv_y                             ); glVertex2d(x + ui_texture -> tile_size   , y);
        glTexCoord2d(uv_x + ui_texture -> atlas_uvs.x, uv_y + ui_texture -> atlas_uvs.y ); glVertex2d(x + ui_texture -> tile_size   , y + ui_texture -> tile_size);
        glTexCoord2d(uv_x                            , uv_y + ui_texture -> atlas_uvs.y ); glVertex2d(x                             , y + ui_texture -> tile_size);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void rendering_draw_healthbar(uint health, Texture* ui_texture, uint health_atlas1, uint health_atlas2){
    double uv_x, uv_y;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    texture_bind(ui_texture, 0);

    uint atlas_index = health_atlas1;
    for(uint i = 0; i < 10; ++i){

        atlas_index = health >= (i + 1) ? health_atlas1 : health_atlas2;
        uv_x = (atlas_index % (ui_texture -> width / ui_texture -> tile_size)) * ui_texture -> atlas_uvs.x;
        uv_y = (atlas_index / (ui_texture -> height / ui_texture -> tile_size)) * ui_texture -> atlas_uvs.y;

        glBegin(GL_QUADS);{
            glTexCoord2d(uv_x                            , uv_y                             ); glVertex2d(2 + (i * ui_texture -> tile_size)                             , 2);
            glTexCoord2d(uv_x + ui_texture -> atlas_uvs.x, uv_y                             ); glVertex2d(2 + (i * ui_texture -> tile_size) + ui_texture -> tile_size   , 2);
            glTexCoord2d(uv_x + ui_texture -> atlas_uvs.x, uv_y + ui_texture -> atlas_uvs.y ); glVertex2d(2 + (i * ui_texture -> tile_size) + ui_texture -> tile_size   , 2 + ui_texture -> tile_size);
            glTexCoord2d(uv_x                            , uv_y + ui_texture -> atlas_uvs.y ); glVertex2d(2 + (i * ui_texture -> tile_size)                             , 2 + ui_texture -> tile_size);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void rendering_draw_hud(Entity_Player* player, Texture* ui_texture_sheet){

    //Draw cursor
    rendering_draw_healthbar(player -> health, ui_texture_sheet, 1, 2);
    rendering_draw_cursor(ui_texture_sheet, 0);
}

Coord2d rendering_viewport_to_world_pos(Camera* cam, Coord2d coord){
    Coord2d position;

    position.x = (coord.x / g_video_mode.scale) + (cam -> position.x - (g_video_mode.viewport.x / 2));
    position.y = (coord.y / g_video_mode.scale) + (cam -> position.y - (g_video_mode.viewport.y / 2));

    return position;
}