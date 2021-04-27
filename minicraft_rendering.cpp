#include "minicraft.h"

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
    gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress);

    //Set up ortho projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window_x, window_y, 0, 1, -1);
    glMatrixMode(GL_PROJECTION); //TODO: why is this here twice.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return windowptr;
}

void rendering_draw_chunk(Chunk* chunk, Texture* atlas_texture, Camera* camera){

    int chunk_x = chunk -> pos_x * (16 * 16) - camera -> pos_x;
    int chunk_y = chunk -> pos_y * (16 * 16) - camera -> pos_y;

    for(int y = 0; y < 16; y++){
        for(int x = 0; x < 16; x++){

            if(chunk_x + (x * 16) > 768 || chunk_x + (x * 16) < -16)
                continue;
            if(chunk_y + (y * 16) > 512 || chunk_y + (y * 16) < -16)
                continue;

            uint texture_coord_x = BlockRegistry[chunk -> foreground_tiles[(y * 16) + x]] -> atlas_index % (atlas_texture -> width / TEXTURE_TILE_RES);
            uint texture_coord_y = BlockRegistry[chunk -> foreground_tiles[(y * 16) + x]] -> atlas_index  / (atlas_texture -> width / TEXTURE_TILE_RES);

            double texture_uv_x = atlas_texture -> atlas_uv_dx * texture_coord_x;
            double texture_uv_y = atlas_texture -> atlas_uv_dy * texture_coord_y;

            texture_bind(atlas_texture, 0);
            glEnable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);{
                glTexCoord2d(texture_uv_x,                                  texture_uv_y);                                  glVertex2i(chunk_x + (x * 16),        chunk_y + (y * 16));
                glTexCoord2d(texture_uv_x + atlas_texture -> atlas_uv_dx,   texture_uv_y);                                  glVertex2i(chunk_x + (x * 16) + 16,   chunk_y + (y * 16));
                glTexCoord2d(texture_uv_x + atlas_texture -> atlas_uv_dx,   texture_uv_y + atlas_texture -> atlas_uv_dy);   glVertex2i(chunk_x + (x * 16) + 16,   chunk_y + (y * 16) + 16);
                glTexCoord2d(texture_uv_x,                                  texture_uv_y + atlas_texture -> atlas_uv_dy);   glVertex2i(chunk_x + (x * 16),        chunk_y + (y * 16) + 16);
            }
            glEnd();
            glDisable(GL_TEXTURE_2D);

        }
    }
}