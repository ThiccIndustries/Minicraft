/*
 * Created by MajesticWaffle on 5/19/22.
 * Copyright (c) 2022 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */

#ifndef PLAYER
#define PLAYER

#include "geode.h"
#define COMP_PLAYER 3

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

        stamina_timer = time_timer_start(0);
        collect_timer = time_timer_start(0);
        attack_timer  = time_timer_start(0);
        c.type = COMP_PLAYER;

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
        };

        c.on_death = [](Entity* e, Component* c){
            Player* player = (Player*)c;
            Transform* transform = e -> transform;
            Renderer* renderer = (Renderer*)entity_get_component(e, COMP_RENDERER);
            Collider* collider = (Collider*)entity_get_component(e, COMP_COLLIDER);
            e -> health = -1;

            int atlas_index = renderer -> atlas_index + 27;
            transform -> velocity = {0, 0};
            renderer -> atlas_index = atlas_index;
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