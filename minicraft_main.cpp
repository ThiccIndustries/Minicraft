/*
 * Created by MajesticWaffle on 4/26/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

int main(int argc, char* argv[]){
    GLFWwindow* windowptr = rendering_init_opengl(RENDER_WINX, RENDER_WINY, 4);

    std::string game_path(argv[0]);
    int lastDir = game_path.find_last_of('/');

    std::string path = game_path.substr(0, lastDir) + "/terrain.bmp";

    std::cout << path.c_str() << std::endl;
    Image* img = texture_load_bmp(path.c_str());
    Texture* tex = texture_generate(img, TEXTURE_MULTIPLE);
    std::cout << tex->atlas_uv_dy << " " << tex->atlas_uv_dx << std::endl;

    Entity_Player* player = new Entity_Player;

    entity_register_entity((Entity*)player);
    player -> camera = new Camera;

    std::cout << ((Entity*)player) -> id << std::endl;
    glfwSwapInterval(0);

    input_register_callbacks(windowptr);

    uint index = 0;

    while(!glfwWindowShouldClose(windowptr)){
        input_poll_input();
        time_update_time(glfwGetTime());

        //std::cout << time_get_framerate() << std::endl;

        world_populate_chunk_buffer(player -> camera);
        rendering_draw_chunk_buffer(tex, player -> camera);

        if(input_get_button(GLFW_MOUSE_BUTTON_1)){
            Coord2d worldspace_pos = rendering_viewport_to_world_pos(windowptr, player -> camera, m_posx, m_posy);
            int cx = (int)worldspace_pos.x / 256;
            int cy = (int)worldspace_pos.y / 256;

            int ty = ((int)worldspace_pos.x % 256) / 16;
            int tx = ((int)worldspace_pos.y % 256) / 16;

            world_modify_chunk(cx, cy, ty, tx, index);
        }

        if(input_get_key_down(GLFW_KEY_Q)){
            if(index > 0) index--;
        }
        if(input_get_key_down(GLFW_KEY_E)){
            index++;
        }

        player -> camera -> position.x += ((input_get_key(GLFW_KEY_D) ? 1 : 0) - (input_get_key(GLFW_KEY_A) ? 1 : 0)) * mTime -> delta * 256;
        player -> camera -> position.y += ((input_get_key(GLFW_KEY_S) ? 1 : 0) - (input_get_key(GLFW_KEY_W) ? 1 : 0)) * mTime -> delta * 256;

        glfwSwapBuffers(windowptr);
    }

    glfwDestroyWindow(windowptr);
    glfwTerminate();

    return 0;
}

