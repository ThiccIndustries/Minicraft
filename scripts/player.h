/*
 * Created by MajesticWaffle on 5/19/22.
 * Copyright (c) 2022 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */

#ifndef PLAYER
#define PLAYER
#define COMP_PLAYER 5

typedef struct Player{
    Component c;
    double  walk_speed, run_speed;
    uint    range;
    int    stamina;

    uint    tmp_debug;
    uint    stamina_delay,  collect_delay,  attack_delay;
    Timer  *stamina_timer, *collect_timer, *attack_timer;

    Player(){
        walk_speed  = ((2.0 * 16) / TIME_TPS);
        run_speed   = ((3.5 * 16) / TIME_TPS);
        range   = 3;

        stamina = 10;
        tmp_debug = 0;

        stamina_delay = TIME_TPS;
        collect_delay = TIME_TPS / 4;
        attack_delay  = TIME_TPS / 4;

        c.type = COMP_PLAYER;

        c.on_create = [](Entity* e, Component* c){
            auto player = e;
            auto player_renderer = entity_get_component<Renderer>(e);
            auto player_col = entity_get_component<Collider>(e);
            auto player_cmp = (Player*)c;
            player_cmp -> stamina_timer = time_timer_start(0);
            player_cmp -> collect_timer = time_timer_start(0);
            player_cmp -> attack_timer  = time_timer_start(0);

            player->health = 10;
            player_renderer->atlas_texture =
                    texture_load(get_resource_path(g_game_path, "resources/entities/player.bmp"), TEXTURE_MULTIPLE, 16);
            player_renderer->sheet_type = SHEET_UDH;
            player_renderer->frame_count = 4;
            player_renderer->frame_order = new uint[]{0, 1, 0, 2};
            player_renderer->animation_rate  = TIME_TPS;
            player_col->col_bounds = {{5,10}, {11, 15}};
            player_col->hit_bounds = {{2, 0}, {14, 15}};
        };

        c.on_tick  = [](Entity* e, Component* c){
            Player* player = (Player*)c;

            if(time_timer_finished(player -> collect_timer)) {
                Coord2d worldspace_pos = rendering_viewport_to_world_pos(e, input_mouse_position());

                Coord2i chunk{(int) floor(worldspace_pos.x / 256.0),
                              (int) floor(worldspace_pos.y / 256.0)};

                Coord2i tile{(int) (worldspace_pos.x - (chunk.x * 256)) / 16,
                             (int) (worldspace_pos.y - (chunk.y * 256)) / 16};

                if (g_debug) {
                    std::cout << "c:" << chunk.x << "," << chunk.y << " t:" << tile.x << "," << tile.y << std::endl;
                }
            }

            if(time_timer_finished(player -> stamina_timer)) {
                player->stamina = clampi(
                        input_get_key(GLFW_KEY_LEFT_SHIFT) ? (player->stamina - 1) : (player->stamina + 1),
                        0, 10);

                player->stamina_timer = time_timer_start(TIME_TPS);
            }

            double speed = (input_get_key(GLFW_KEY_LEFT_SHIFT) && player -> stamina != 0) ? player -> run_speed : player -> walk_speed;
            double dx = speed * ((input_get_key(GLFW_KEY_D) ? 1 : 0) - (input_get_key(GLFW_KEY_A) ? 1 : 0));
            double dy = speed * ((input_get_key(GLFW_KEY_S) ? 1 : 0) - (input_get_key(GLFW_KEY_W) ? 1 : 0));
            e -> transform -> velocity = {dx, dy};
        };

        c.on_death = [](Entity* e, Component* c){
            Player* player = (Player*)c;
            Transform* transform = e -> transform;
            Renderer* renderer = entity_get_component<Renderer>(e);
            Collider* collider = entity_get_component<Collider>(e);
            e -> health = -1;

            transform -> velocity = {0, 0};
            transform -> direction = DIRECTION_SOUTH;
            transform -> move_state = ENT_STATE_STATIONARY;
            renderer -> atlas_index = 9;
            collider -> col_bounds = {};
            collider -> hit_bounds = {};
            renderer -> animation_rate = 9999;
            player -> run_speed = 0;
            player -> walk_speed = 0;

            time_callback_start(TIME_TPS * 5, [](void* v){
                Entity* e = (Entity*)v;
                error("Game Over.", "Player died on tick: " + std::to_string(g_time -> tick));
                entity_delete(e -> id);
                g_time -> paused = true;
            }, e);
        };
    }
}Player;

#endif