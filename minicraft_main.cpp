#include <cstring>
#include "minicraft.h"

int main(int argc, char* argv[]){
    GLFWwindow* windowptr = rendering_init_opengl(768, 512, 3);

    std::string path = std::string("/home/trevorskupien/Documents/GitHub/Minicraft/cmake-build-debug/terrain.bmp");

    std::cout << path.c_str() << std::endl;
    Image* img = texture_load_bmp(path.c_str());
    Texture* tex = texture_generate(img, TEXTURE_MULTIPLE);
    std::cout << tex->atlas_uv_dy << " " << tex->atlas_uv_dx << std::endl;

    Camera* camera = new Camera;
    camera -> pos_x = 0;
    camera -> pos_y = 0;

    int chunk_prev_x;

    glfwSwapInterval(0);

    input_register_callbacks(windowptr);

    while(!glfwWindowShouldClose(windowptr)){
        input_poll_input();

        if(chunk_prev_x != camera -> pos_x / 256) {
            world_populate_chunk_buffer(camera);
            chunk_prev_x = camera -> pos_x / 256;
            //std::cout << "x: " << camera -> pos_x << " y: " << camera -> pos_y << std::endl;
        }

        for(int i = 0; i < RENDER_DISTANCE * RENDER_DISTANCE * 4; i++){
            rendering_draw_chunk(ChunkBuffer[i], tex, camera);
        }

        //if(input_get_key(GLFW_KEY_D))
            camera -> pos_x ++;

        glfwSwapBuffers(windowptr);
    }

    glfwDestroyWindow(windowptr);
    glfwTerminate();

    return 0;
}

