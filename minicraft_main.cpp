/*
 * Created by MajesticWaffle on 4/26/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

std::string game_root_path = "";

int main(int argc, char* argv[]){
    game_root_path = argv[0];

    std::string saveName = "test";

    GLFWwindow* windowptr = rendering_init_opengl(RENDER_WINX, RENDER_WINY, 4);

    //Having to create an Image* then pass it to create a Texture* is dumb.
    //TODO: Unify this. (Why would you ever need an Image and NOT a Texture)
    Image* img = texture_load_bmp(get_resource_path(game_root_path, "terrain.bmp").c_str());
    Texture* tex = texture_generate(img, TEXTURE_MULTIPLE);

    //Create player
    Entity_Player* player = (Entity_Player*)entity_create((Entity*)new Entity_Player);

    //Disable Vsync
    glfwSwapInterval(0);

    input_register_callbacks(windowptr);

    uint index = 0;

    while(!glfwWindowShouldClose(windowptr)){
        input_poll_input();
        time_update_time(glfwGetTime());

        world_populate_chunk_buffer(saveName, &player -> camera);
        rendering_draw_chunk_buffer(tex, &player -> camera);

        if(input_get_button(GLFW_MOUSE_BUTTON_1)){
            Coord2d worldspace_pos = rendering_viewport_to_world_pos(windowptr, &player -> camera, m_posx, m_posy);
            int cx = (int)worldspace_pos.x / 256;
            int cy = (int)worldspace_pos.y / 256;

            int ty = ((int)worldspace_pos.x % 256) / 16;
            int tx = ((int)worldspace_pos.y % 256) / 16;

            world_modify_chunk(saveName, cx, cy, ty, tx, index);
        }

        if(input_get_key_down(GLFW_KEY_Q)){
            if(index > 0) index--;
        }

        if(input_get_key_down(GLFW_KEY_E)){
            index++;
        }

        player -> camera.position.x += ((input_get_key(GLFW_KEY_D) ? 1 : 0) - (input_get_key(GLFW_KEY_A) ? 1 : 0)) * mTime -> delta * 256;
        player -> camera.position.y += ((input_get_key(GLFW_KEY_S) ? 1 : 0) - (input_get_key(GLFW_KEY_W) ? 1 : 0)) * mTime -> delta * 256;

        glfwSwapBuffers(windowptr);
    }

    entity_delete(0); //Delete player
    glfwDestroyWindow(windowptr); //Destroy window
    glfwTerminate();

    return 0;
}

