/*
 * Created by MajesticWaffle on 4/26/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"
#include <cmath>

std::string g_game_path;

void tick();

int main(int argc, char* argv[]){
    g_game_path = argv[0];
    std::string saveName = "test";

    GLFWwindow* windowptr = rendering_init_opengl(RENDER_WINX, RENDER_WINY, RENDER_SCALE);

    //Load textures
    Font* font = new Font{
            texture_load_bmp(get_resource_path(g_game_path, "resources/font.bmp").c_str(), TEXTURE_MULTIPLE, 8),
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!\"#$%&\'()*+,-./:;<=>?@[\\]^_ {|}~0123456789"
    };
    g_def_font = font;

    Texture* terr = texture_load_bmp(get_resource_path(g_game_path, "resources/terrain.bmp"), TEXTURE_MULTIPLE, 16);
    Texture* ui   = texture_load_bmp(get_resource_path(g_game_path, "resources/ui.bmp"), TEXTURE_MULTIPLE, 8);
    Texture* ent  = texture_load_bmp(get_resource_path(g_game_path, "resources/entity.bmp"), TEXTURE_MULTIPLE, 16);

    Entity_Player* player = (Entity_Player*) entity_create( ((Entity*)new Entity_Player)); //Entity 0
    Entity* test_ent = entity_create(new Entity{ .position = {50, 50}, .bounds = {{5,10}, {11, 15}} });
    //Disable Vsync
    glfwSwapInterval(0);

    input_register_callbacks(windowptr);

    uint index = 0;

    Timer* timer = nullptr;

    time_set_tick_callback(&tick);

    while(!glfwWindowShouldClose(windowptr)){
        input_poll_input();
        time_update_time(glfwGetTime());

        world_populate_chunk_buffer(saveName,   (Entity*)player);
        rendering_draw_chunk_buffer(terr,       (Entity*)player);
        rendering_draw_entities(ent,            (Entity*)player);

        rendering_draw_hud(player, ui);


        if(input_get_button_up(GLFW_MOUSE_BUTTON_1)) {
            time_timer_cancel(timer);
        }

        if(time_timer_finished(timer)){
            Coord2d worldspace_pos = rendering_viewport_to_world_pos((Entity*)player, input_mouse_position());

            Coord2i chunk{ (int)floor(worldspace_pos.x / 256.0),
                           (int)floor(worldspace_pos.y / 256.0) };

            Coord2i tile{ (int)(worldspace_pos.x - (chunk.x * 256)) / 16,
                          (int)(worldspace_pos.y - (chunk.y * 256)) / 16 };

#ifdef DEBUG
            std::cout << "c:" << chunk.x << "," << chunk.y << " t:" << tile.x << "," << tile.y <<std::endl;
#endif
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

        double dx = ((input_get_key(GLFW_KEY_D) ? 1 : 0) - (input_get_key(GLFW_KEY_A) ? 1 : 0)) * g_time -> delta * (3 * 16);
        double dy = ((input_get_key(GLFW_KEY_S) ? 1 : 0) - (input_get_key(GLFW_KEY_W) ? 1 : 0)) * g_time -> delta * (3 * 16);

        player -> e.move_state = MOVE_STATIONARY;
        entity_move((Entity*)player, {dx, dy}, true);

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

