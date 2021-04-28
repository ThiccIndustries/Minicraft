#include <cstring>
#include "minicraft.h"

int main(int argc, char* argv[]){
    GLFWwindow* windowptr = rendering_init_opengl(RENDER_WINX, RENDER_WINY, 4);

    std::string path = std::string("/home/trevorskupien/Documents/GitHub/Minicraft/cmake-build-debug/terrain.bmp");

    std::cout << path.c_str() << std::endl;
    Image* img = texture_load_bmp(path.c_str());
    Texture* tex = texture_generate(img, TEXTURE_MULTIPLE);
    std::cout << tex->atlas_uv_dy << " " << tex->atlas_uv_dx << std::endl;

    Camera* camera = new Camera;
    camera -> pos_x = 0;
    camera -> pos_y = 0;

    int chunk_prev_x;
    int chunk_prev_y;

    glfwSwapInterval(0);

    input_register_callbacks(windowptr);

    uint index = 0;

    while(!glfwWindowShouldClose(windowptr)){
        input_poll_input();

        if(chunk_prev_x != camera -> pos_x / 256 || chunk_prev_y != camera -> pos_y / 256) {
            world_populate_chunk_buffer(camera);
            chunk_prev_x = camera -> pos_x / 256;
            chunk_prev_y = camera -> pos_y / 256;
            //std::cout << "x: " << camera -> pos_x << " y: " << camera -> pos_y << std::endl;
        }

        for(int i = 0; i < RENDER_DISTANCE * RENDER_DISTANCE * 4; i++){
            rendering_draw_chunk(ChunkBuffer[i], tex, camera);
        }

        if(input_get_button(GLFW_MOUSE_BUTTON_1)){
            Coord2d* worldspace_pos = rendering_viewport_to_world_pos(windowptr, camera, m_posx, m_posy);
            int cx = (int)worldspace_pos -> x / 256;
            int cy = (int)worldspace_pos -> y / 256;

            std::cout << worldspace_pos -> x << " " << worldspace_pos -> y << std::endl;

            int ty = ((int)worldspace_pos -> x % 256) / 16;
            int tx = ((int)worldspace_pos -> y % 256) / 16;

            world_modify_chunk(cx, cy, ty, tx, index);
        }

        if(input_get_key_down(GLFW_KEY_Q)){
            if(index > 0) index--;
        }
        if(input_get_key_down(GLFW_KEY_E)){
            index++;
        }

        camera -> pos_x += (input_get_key(GLFW_KEY_D) ? 1 : 0) - (input_get_key(GLFW_KEY_A) ? 1 : 0);
        camera -> pos_y += (input_get_key(GLFW_KEY_S) ? 1 : 0) - (input_get_key(GLFW_KEY_W) ? 1 : 0);
        glfwSwapBuffers(windowptr);
    }

    glfwDestroyWindow(windowptr);
    glfwTerminate();

    return 0;
}

