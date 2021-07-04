/*
 * Created by MajesticWaffle on 4/26/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */

#include "minicraft.h"
#include <cmath>

std::string g_game_path;
bool g_debug = false;

void tick();

int main(int argc, char* argv[]){
    g_game_path = argv[0];
    std::string saveName = "test";

    GLFWwindow* windowptr = rendering_init_opengl(RENDER_WINX, RENDER_WINY, RENDER_SCALE, 1, 1);

    //Load textures
    Font* font = new Font{
            texture_load_bmp(get_resource_path(g_game_path, "resources/font.bmp"), TEXTURE_MULTIPLE, 8),
            R"(ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!"#$%&'()*+,-./:;<=>?@[\]^_ {|}~0123456789)"
    };
    g_def_font = font;

    Texture* terr = texture_load_bmp(get_resource_path(g_game_path, "resources/terrain.bmp"), TEXTURE_MULTIPLE, 16);
    Texture* ui   = texture_load_bmp(get_resource_path(g_game_path, "resources/ui.bmp"), TEXTURE_MULTIPLE, 8);
    Texture* ent  = texture_load_bmp(get_resource_path(g_game_path, "resources/entity.bmp"), TEXTURE_MULTIPLE, 16);

    Entity_Player* player = (Entity_Player*) entity_create( ((Entity*)new Entity_Player)); //Entity 0
    Entity_Player* p2 = (Entity_Player*) entity_create( (Entity*)new Entity_Player );

    //Entity* test_ent = entity_create(new Entity{ .position = {50, 50}, .bounds = {{5,10}, {11, 15}} });
    //Disable Vsync
    glfwSwapInterval(0);

    input_register_callbacks(windowptr);

    uint index = 0;

    Timer* timer = nullptr;

    time_set_tick_callback(&tick);

    uint fps = 0;
    Timer* t = time_timer_start(TIME_TPS / 2);

    while(!glfwWindowShouldClose(windowptr)){
        input_poll_input();
        time_update_time(glfwGetTime());

        world_populate_chunk_buffer(saveName,   (Entity*)player);
        rendering_draw_chunk_buffer(terr,       (Entity*)player);
        rendering_draw_entities(ent,            (Entity*)player);

        rendering_draw_hud(player, ui);

        if(g_debug){
            if(time_timer_finished(t)){
                t = time_timer_start(TIME_TPS / 4);
                fps = clampi(time_get_framerate(), 0, 9999);
            }


            rendering_draw_text("Fps:" + std::to_string(fps), g_video_mode.ui_scale, font, {255, 255, 255}, {RENDER_WINX - (int)(font -> t -> tile_size * 8 * g_video_mode.ui_scale), 2} );
        }

        if(input_get_key_down(GLFW_KEY_F3)){
            g_debug = !g_debug;
        }
        
        if(input_get_button_up(GLFW_MOUSE_BUTTON_1)) {
            time_timer_cancel(timer);
        }

        if(time_timer_finished(timer)){
            Coord2d worldspace_pos = rendering_viewport_to_world_pos((Entity*)player, input_mouse_position());

            Coord2i chunk{ (int)floor(worldspace_pos.x / 256.0),
                           (int)floor(worldspace_pos.y / 256.0) };

            Coord2i tile{ (int)(worldspace_pos.x - (chunk.x * 256)) / 16,
                          (int)(worldspace_pos.y - (chunk.y * 256)) / 16 };

            if(g_debug){
                std::cout << "c:" << chunk.x << "," << chunk.y << " t:" << tile.x << "," << tile.y <<std::endl;
            }

            world_modify_chunk(saveName, chunk, tile, index);
        }

        if(input_get_button(GLFW_MOUSE_BUTTON_1)){
            timer = time_timer_start(1);
        }

        if(input_get_key_down(GLFW_KEY_Q)){
            index = clampi(index - 1, 0, 22);
            player -> health --;
        }

        if(input_get_key_down(GLFW_KEY_E)){
            index = clampi(index + 1, 0, 22);
            player -> health ++;
        }

        double dx = ((3.0 * 16) / TIME_TPS) * ((input_get_key(GLFW_KEY_D) ? 1 : 0) - (input_get_key(GLFW_KEY_A) ? 1 : 0));
        double dy = ((3.0 * 16) / TIME_TPS) * ((input_get_key(GLFW_KEY_S) ? 1 : 0) - (input_get_key(GLFW_KEY_W) ? 1 : 0));
        player -> e.velocity = {dx, dy};

        glfwSwapBuffers(windowptr);
    }

    entity_delete(0); //Delete player
    glfwDestroyWindow(windowptr); //Destroy window
    glfwTerminate();

    return 0;
}

void tick(){
    entity_tick();
}

