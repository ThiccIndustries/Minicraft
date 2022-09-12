/*
 * Created by MajesticWaffle on 5/19/22.
 * Copyright (c) 2022 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */
#ifndef BONE
#define BONE

#define COMP_BONE 9

typedef struct Bone{
    Component c;

    double  movementSpeed;
    double  lifetime;
    Timer*  lifetime_timer;
    Entity* target;

    Bone() {
        movementSpeed   = ((5.0 * 16) / TIME_TPS);
        lifetime        = 2 * TIME_TPS;
        c.type = COMP_BONE;

        c.on_create = [](Entity* e, Component* c){
            auto e_renderer = entity_get_component<Renderer>(e);
            auto e_col = entity_get_component<Collider>(e);
            auto bone = (Bone*)c;

            bone -> lifetime_timer = nullptr;
            bone -> target = nullptr;

            e_renderer->atlas_texture =
                    texture_load(get_resource_path(g_game_path, "resources/entities/bone.bmp"), TEXTURE_MULTIPLE, 16);
            e_renderer->sheet_type = SHEET_SINGLE;
            e_renderer->frame_count = 2;
            e_renderer->frame_order = new uint[]{0, 1};
            e_renderer->animation_rate = TIME_TPS / 4;
            e_col->col_bounds  = {{2, 2}, {9, 9}};
            e_col->hit_bounds  = {{1, 1}, {10, 10}};
            e->health = 999;
        };

        c.on_tick = [](Entity* e, Component* c){
            Bone* bone = (Bone*)c;
            Collider* col = entity_get_component<Collider>(e);

            Entity* hit = entity_hit(col, e -> transform);
            if( hit != nullptr){
                entity_damage(hit, 1);
                entity_kill(e);
                return;
            }

            //Check for wall hits
            Coord2d check_position = entity_collision(col, e->transform, e->transform->velocity);

            //Bone hit a wall
            if(check_position != e-> transform -> velocity){
                entity_kill(e);
                return;
            }

            if(time_timer_finished( bone -> lifetime_timer )) {
                entity_kill(e);
                return;
            }
        };

        c.on_death = [](Entity* e, Component* c){
            e -> health = -1;
            Bone* bone = (Bone*)e;
            Transform* transform = e -> transform;
            Renderer* renderer = entity_get_component<Renderer>(e);
            Collider* collider = entity_get_component<Collider>(e);

            //Use tick to "randomly" assign sprite
            int atlas_index = (g_time -> tick % 2 == 0) ? 1 : 3;
            transform -> velocity = {0, 0};
            renderer -> atlas_index = atlas_index;
            transform -> direction = DIRECTION_SOUTH;
            collider -> col_bounds = {0,0};
            collider -> hit_bounds = {0,0};
            renderer -> animation_rate = 9999;

            std::cout << e -> id << std::endl;

           time_callback_start(TIME_TPS / 2, [](void* v){

                Entity* e = (Entity*)v;

                entity_delete(e -> id);
            }, e);

        };
    }
}Bone;

#endif