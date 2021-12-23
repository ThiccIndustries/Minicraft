/*
 * Created by MajesticWaffle on 4/26/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */

#include "minicraft.h"
#include <cmath>

Save* g_save = nullptr;
std::string g_game_path;
bool g_debug = false;

void tick();
Texture* terr;

Texture* texture_callback(Chunk* c);

Entity_Player *player = nullptr;

int main(int argc, char* argv[]){
    g_game_path = argv[0];
    g_save = world_load_game("test");

    GLFWwindow* windowptr = rendering_init_opengl(320, 240, 1, 1, 1);

    //Load textures
    Font* font = new Font{
            texture_load_bmp(get_resource_path(g_game_path, "resources/font.bmp"), TEXTURE_MULTIPLE, 8),
            R"(ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!"#$%&'()*+,-./:;<=>?@[\]^_ {|}~0123456789)"
    };
    g_def_font = font;

    terr = texture_load_bmp(get_resource_path(g_game_path, "resources/terrain.bmp"), TEXTURE_MULTIPLE | TEXTURE_STORE, 16);
    Texture* ui   = texture_load_bmp(get_resource_path(g_game_path, "resources/ui.bmp"), TEXTURE_MULTIPLE, 8);
    Texture* ent  = texture_load_bmp(get_resource_path(g_game_path, "resources/entity.bmp"), TEXTURE_MULTIPLE, 16);

    player = (Entity_Player*) entity_create( (Entity*)new Entity_Player); //Entity 0

    player -> e.position = Coord2d{(double)g_save->player_position.x, (double)g_save->player_position.y};
    Entity_Skeleton* skeleton = (Entity_Skeleton*) entity_create((Entity*)new Entity_Skeleton);

    //Disable Vsync
    glfwSwapInterval(0);

    input_register_callbacks(windowptr);

    time_set_tick_callback(&tick);
    world_set_chunk_callbacks(
            &world_load_chunk,
            &world_unload_chunk,
            &texture_callback
            );

    uint fps = 0;
    float ftime = 0.0;
    Timer* t = time_timer_start(TIME_TPS / 2);

    while(!glfwWindowShouldClose(windowptr)){
        input_poll_input();
        time_update_time(glfwGetTime());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        world_populate_chunk_buffer((Entity*)player);

        rendering_draw_chunk_buffer((Entity*)player);
        rendering_draw_entities(ent,       (Entity*)player);
        rendering_draw_bar(player -> e.health,  {0, 0}, ui, 8);
        rendering_draw_bar(player -> stamina,   {0, (int)ui -> tile_size}, ui, 16);

        uint cursor = 0;
        double x, y;
        double scl = g_video_mode.ui_scale;
        x = input_mouse_position().x / (g_video_mode.window_scale * scl);
        y = input_mouse_position().y / (g_video_mode.window_scale * scl);

        Coord2d worldspace_pos = rendering_viewport_to_world_pos((Entity*)player, input_mouse_position());

        Coord2i chunk{(int) floor(worldspace_pos.x / 256.0),
                      (int) floor(worldspace_pos.y / 256.0)};

        Coord2i tile{(int) (worldspace_pos.x - (chunk.x * 256)) / 16,
                     (int) (worldspace_pos.y - (chunk.y * 256)) / 16};

        if(world_get_chunk(chunk) != nullptr && g_block_registry[world_get_chunk(chunk)->overlay_tiles[tile.x + (tile.y * 16)]]->options & TILE_COLLECTABLE)
            cursor = 1;

        rendering_draw_cursor(ui, cursor);
        if(cursor != 0 && distancec2d(worldspace_pos, player -> e.position) > player -> range * 16){
            rendering_draw_text("Range!", g_video_mode.ui_scale, g_def_font, {000, 000, 000}, Coord2d{x + 6, y + 9});
            rendering_draw_text("Range!", g_video_mode.ui_scale, g_def_font, {200, 050, 050}, Coord2d{x + 6, y + 8});
        }

        //if(g_debug){
            if(time_timer_finished(t)){
                t = time_timer_start(TIME_TPS / 4);
                fps = clampi(time_get_framerate(), 0, 99999);
                ftime = g_time -> delta;
            }

        rendering_draw_text("Fps:" + std::to_string(fps), g_video_mode.ui_scale, font, {255, 255, 255}, {g_video_mode.window_resolution.x - (double)(font -> t -> tile_size * 10 * g_video_mode.ui_scale), 0} );
        rendering_draw_text(" us:" + std::to_string(ftime * 1000.0f), g_video_mode.ui_scale, font, {255, 255, 255}, {g_video_mode.window_resolution.x - (double)(font -> t -> tile_size * 10 * g_video_mode.ui_scale), 8} );
        //}

        if(input_get_key_down(GLFW_KEY_F3)){
            g_debug = !g_debug;
        }

        if(input_get_button_up(GLFW_MOUSE_BUTTON_1)) {
            time_timer_cancel(player -> collect_timer);
        }

        if(input_get_button_down(GLFW_MOUSE_BUTTON_1)){
            player -> collect_timer = time_timer_start(player -> collect_delay);
        }

        if(input_get_key_down(GLFW_KEY_ESCAPE)){
            g_time -> paused = !g_time -> paused;
        }

        if(input_get_key_down(GLFW_KEY_Q))
            player -> tmp_debug = clampi(player -> tmp_debug - 1, 21, 24);

        if(input_get_key_down(GLFW_KEY_E))
            player -> tmp_debug = clampi(player -> tmp_debug + 1, 21, 24);


        double speed = (input_get_key(GLFW_KEY_LEFT_SHIFT) && player -> stamina != 0) ? player -> run_speed : player -> walk_speed;

        double dx = speed * ((input_get_key(GLFW_KEY_D) ? 1 : 0) - (input_get_key(GLFW_KEY_A) ? 1 : 0));
        double dy = speed * ((input_get_key(GLFW_KEY_S) ? 1 : 0) - (input_get_key(GLFW_KEY_W) ? 1 : 0));
        player -> e.velocity = {dx, dy};

        glfwSwapBuffers(windowptr);
    }
    world_save_game(g_save);

    entity_delete(0); //Delete player
    glfwDestroyWindow(windowptr); //Destroy window
    glfwTerminate();

    return 0;
}

void tick(){
    entity_tick();
    g_save -> player_position = Coord2i{(int)player -> e.position.x , (int)player -> e.position.y};
}

Texture* texture_callback(Chunk* c){
    return terr;
}


