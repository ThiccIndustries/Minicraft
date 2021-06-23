/*
 * Created by MajesticWaffle on 4/26/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

Video_Mode g_video_mode;
Font* g_def_font;

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
    glMatrixMode(GL_PROJECTION);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Mouse mode
    glfwSetInputMode(windowptr, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    g_video_mode.viewport    = {(int)window_x, (int)window_y};
    g_video_mode.scale       = scale;
    g_video_mode.windowptr   = windowptr;
    return windowptr;
}

void rendering_draw_chunk(Chunk* chunk, Texture* atlas_texture, Entity* viewpoint_e){

    double chunk_x = chunk -> pos.x * (16 * 16) - (viewpoint_e -> position.x + (viewpoint_e -> camera.position.x) - (g_video_mode.viewport.x / 2));
    double chunk_y = chunk -> pos.y * (16 * 16) - (viewpoint_e -> position.y + (viewpoint_e -> camera.position.y) - (g_video_mode.viewport.y / 2));

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

            bool inv_x = g_block_registry[chunk -> foreground_tiles[(y * 16) + x]]->options & TILE_TEX_FLIP_X;
            bool inv_y = g_block_registry[chunk -> foreground_tiles[(y * 16) + x]]->options & TILE_TEX_FLIP_Y;

            if(inv_y == true)
                std::cout << (uint)chunk -> foreground_tiles[(y * 16) + x] << std::endl;
            texture_bind(atlas_texture, 0);
            glEnable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);{
                glTexCoord2d(texture_uv_x + atlas_texture -> atlas_uvs.x * inv_x,    texture_uv_y + atlas_texture -> atlas_uvs.y * inv_y);  glVertex2d(chunk_x + (x * 16),        chunk_y + (y * 16));
                glTexCoord2d(texture_uv_x + atlas_texture -> atlas_uvs.x * !inv_x,   texture_uv_y + atlas_texture -> atlas_uvs.y * inv_y);  glVertex2d(chunk_x + (x * 16) + 16,   chunk_y + (y * 16));
                glTexCoord2d(texture_uv_x + atlas_texture -> atlas_uvs.x * !inv_x,   texture_uv_y + atlas_texture -> atlas_uvs.y * !inv_y); glVertex2d(chunk_x + (x * 16) + 16,   chunk_y + (y * 16) + 16);
                glTexCoord2d(texture_uv_x + atlas_texture -> atlas_uvs.x * inv_x,    texture_uv_y + atlas_texture -> atlas_uvs.y * !inv_y); glVertex2d(chunk_x + (x * 16),        chunk_y + (y * 16) + 16);
            }
            glEnd();
            glDisable(GL_TEXTURE_2D);

            #ifdef DEBUG
                if(g_block_registry[chunk -> foreground_tiles[(y * 16) + x]] -> options & TILE_SOLID) {
                    glColor3ub(255, 0, 0);
                    glLineWidth(2);
                    glBegin(GL_LINES);
                    {
                        glVertex2d(chunk_x + (x * 16),      chunk_y + (y * 16));
                        glVertex2d(chunk_x + (x * 16) + 16, chunk_y + (y * 16));

                        glVertex2d(chunk_x + (x * 16),      chunk_y + (y * 16) + 16);
                        glVertex2d(chunk_x + (x * 16) + 16, chunk_y + (y * 16) + 16);

                        glVertex2d(chunk_x + (x * 16), chunk_y + (y * 16));
                        glVertex2d(chunk_x + (x * 16), chunk_y + (y * 16) + 16);

                        glVertex2d(chunk_x + (x * 16) + 16, chunk_y + (y * 16));
                        glVertex2d(chunk_x + (x * 16) + 16, chunk_y + (y * 16) + 16);
                    }
                    glEnd();
                    glColor3ub(255, 255, 255);
                }
            #endif


        }
    }
}

void rendering_draw_entity(Entity* entity, Texture* atlas_texture, Entity* viewport_e){
    double entity_x = entity -> position.x - (viewport_e -> position.x + (viewport_e -> camera.position.x) - (g_video_mode.viewport.x / 2));
    double entity_y = entity -> position.y - (viewport_e -> position.y + (viewport_e -> camera.position.y) - (g_video_mode.viewport.y / 2));

    //Get texture for current state
    uint index = entity -> atlas_index;
    bool inv_x = false;

    switch(entity -> direction){
        case DIRECTION_NORTH:
            index += 1;
            inv_x = false;
            break;
        case DIRECTION_EAST:
            index += 2;
            inv_x = true;
            break;
        case DIRECTION_SOUTH:
            index += 0;
            inv_x = false;
            break;
        case DIRECTION_WEST:
            index += 2;
            inv_x = false;
            break;
    }

    switch(entity -> move_state){
        case MOVE_ACTIVE:
            int anim_tick = g_time -> tick % entity -> animation_rate;
            int anim_rate_d = entity -> animation_rate / 4;
            if(anim_tick >= anim_rate_d * 3)
                index += 8;
            else if(anim_tick >= anim_rate_d * 2)
                index += 0;
            else if(anim_tick >= anim_rate_d * 1)
                index += 16;

    }

    //std::cout << inv_x << std::endl;
    uint texture_coord_x = index % (atlas_texture -> width / atlas_texture -> tile_size);
    uint texture_coord_y = index / (atlas_texture -> width / atlas_texture -> tile_size);

    double texture_uv_x = atlas_texture -> atlas_uvs.x * texture_coord_x;
    double texture_uv_y = atlas_texture -> atlas_uvs.y * texture_coord_y;

    //TODO: sprites
    texture_bind(atlas_texture, 0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    glBegin(GL_QUADS);{
        glTexCoord2d(texture_uv_x + (atlas_texture -> atlas_uvs.x * inv_x),     texture_uv_y);                                 glVertex2d(entity_x,        entity_y);
        glTexCoord2d(texture_uv_x + (atlas_texture -> atlas_uvs.x * !inv_x),    texture_uv_y);                                 glVertex2d(entity_x + 16,   entity_y);
        glTexCoord2d(texture_uv_x + (atlas_texture -> atlas_uvs.x * !inv_x),    texture_uv_y + atlas_texture -> atlas_uvs.y);  glVertex2d(entity_x + 16,   entity_y + 16);
        glTexCoord2d(texture_uv_x + (atlas_texture -> atlas_uvs.x * inv_x),     texture_uv_y + atlas_texture -> atlas_uvs.y);  glVertex2d(entity_x,        entity_y + 16);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

#ifdef DEBUG
    Coord2d entity_bounds_p1{
        entity_x + entity -> bounds.p1.x,
        entity_y + entity -> bounds.p1.y
    };
    Coord2d entity_bounds_p2{
            entity_x + entity -> bounds.p2.x,
            entity_y + entity -> bounds.p2.y
    };

    glColor3ub(255, 0, 0);
    glBegin(GL_LINES);{
        glVertex2d(entity_bounds_p1.x, entity_bounds_p1.y);
        glVertex2d(entity_bounds_p2.x, entity_bounds_p1.y);

        glVertex2d(entity_bounds_p1.x, entity_bounds_p2.y);
        glVertex2d(entity_bounds_p2.x, entity_bounds_p2.y);

        glVertex2d(entity_bounds_p1.x, entity_bounds_p1.y);
        glVertex2d(entity_bounds_p1.x, entity_bounds_p2.y);

        glVertex2d(entity_bounds_p2.x, entity_bounds_p1.y);
        glVertex2d(entity_bounds_p2.x, entity_bounds_p2.y);
    }
    glEnd();
    glColor3ub(255, 255, 255);
#endif

}

void rendering_draw_entities(Texture* atlas_texture, Entity* viewport_e){
    for(int i = g_entity_highest_id; i >= 0; i--){
        rendering_draw_entity(g_entity_registry[i], atlas_texture, viewport_e);
    }
}

void rendering_draw_chunk_buffer(Texture* atlas_texture, Entity* viewport_e){
    for(int i = 0; i < RENDER_DISTANCE * RENDER_DISTANCE * 4; ++i){
        rendering_draw_chunk(g_chunk_buffer[i], atlas_texture, viewport_e);
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

    x = input_mouse_position().x / g_video_mode.scale;
    y = input_mouse_position().y / g_video_mode.scale;

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

void rendering_draw_dialog(const std::string& title, const std::string& message, Font* font){
    //How many pixels to offset message by to center it
    uint offset = ((RENDER_WINX / 2) - (message.length() * font -> t -> tile_size) / 2);
    uint title_offset = ((RENDER_WINX / 2) - (title.length() * font -> t -> tile_size) / 2);

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);{
        glVertex2i(offset - 2                                                   , (RENDER_WINY / 2) - 2 - (int)(font -> t -> tile_size / 2) - (3 * font -> t -> tile_size));
        glVertex2i(offset + 2 + ((message.length()) * font -> t -> tile_size)   , (RENDER_WINY / 2) - 2 - (int)(font -> t -> tile_size / 2) - (3 * font -> t -> tile_size));
        glVertex2i(offset + 2 + ((message.length()) * font -> t -> tile_size)   , (RENDER_WINY / 2) + 2 - (int)(font -> t -> tile_size / 2) + 2 * font -> t -> tile_size);
        glVertex2i(offset - 2                                                   , (RENDER_WINY / 2) + 2 - (int)(font -> t -> tile_size / 2) + 2 * font -> t -> tile_size);
    }
    glEnd();
    glColor3ub(255, 255, 255);

    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);{
        glVertex2i(offset                                                      , (RENDER_WINY / 2) - (int)(font -> t -> tile_size / 2) - (3 * font -> t -> tile_size));
        glVertex2i(offset + ((message.length()) * font -> t -> tile_size)      , (RENDER_WINY / 2) - (int)(font -> t -> tile_size / 2) - (3 * font -> t -> tile_size));
        glVertex2i(offset + ((message.length()) * font -> t -> tile_size)      , (RENDER_WINY / 2) - (int)(font -> t -> tile_size / 2) + 2 * font -> t -> tile_size);
        glVertex2i(offset                                                      , (RENDER_WINY / 2) - (int)(font -> t -> tile_size / 2) + 2 * font -> t -> tile_size);
    }
    glEnd();
    glColor3ub(255, 255, 255);

    rendering_draw_text(title, 1, font, {255, 255, 255}, {(int)title_offset, (RENDER_WINY / 2) - (int)(font -> t -> tile_size / 2) - (2* (int)font -> t -> tile_size)});
    rendering_draw_text(message, 1, font, {255, 255, 255}, {(int)offset, (RENDER_WINY / 2) - (int)(font -> t -> tile_size / 2)});
}
void rendering_draw_hud(Entity_Player* player, Texture* ui_texture_sheet){

    //Draw cursor
    rendering_draw_healthbar(player -> health, ui_texture_sheet, 1, 2);
    rendering_draw_cursor(ui_texture_sheet, 0);
}

Coord2d rendering_viewport_to_world_pos(Entity* viewport_e, Coord2d coord){
    Coord2d position;

    position.x = (coord.x / g_video_mode.scale) + (viewport_e -> position.x + (viewport_e -> camera.position.x) - (g_video_mode.viewport.x / 2));
    position.y = (coord.y / g_video_mode.scale) + (viewport_e -> position.y + (viewport_e -> camera.position.y) - (g_video_mode.viewport.y / 2));

    return position;
}