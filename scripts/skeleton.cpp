/*
 * Created by MajesticWaffle on 5/19/22.
 * Copyright (c) 2022 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */
#ifndef SKELETON
#define SKELETON

#include "geode.h"
#include "enemy.cpp"
#include "bone.cpp"

#define COMP_SKELETON 6

typedef struct Skeleton{
    Enemy e;

    Skeleton(){
        /*e.e.health        = 10;*/
        e.movementSpeed = ((1.5 * 16) / TIME_TPS);
        e.attack_range  = 5 * 16;
        e.follow_range  = 5 * 16;
        e.attack_time   = 64;
        e.c.type = COMP_SKELETON;

        e.c.on_tick   = [](Entity* e, Component* c){
            Enemy* enemy = (Enemy*)c;
            enemy -> c.on_tick(e, c);
            //Attack
            if(!(enemy -> attack_timer == nullptr || time_timer_finished(enemy -> attack_timer)))
                return;

            if(enemy -> target == nullptr)
                return;

            enemy -> attack_timer = time_timer_start(enemy -> attack_time);
            Entity* proj = entity_create();
            auto collider = entity_add_component<Collider>(proj);
            auto renderer = entity_add_component<Renderer>(proj);
            auto bone = entity_add_component<Bone>(proj);

            proj->transform->position = e->transform->position;
            proj->transform->map = e->transform->map;
            bone -> target = enemy -> target;
            bone -> movementSpeed = (4.0 * 16) / TIME_TPS;

            //Recenter coordinate space

            double target_x = bone -> target-> transform -> position.x + (bone -> target-> transform -> velocity.x * distancec2d(proj -> transform -> position, bone -> target -> transform -> position) / bone -> movementSpeed);
            double target_y = bone -> target-> transform -> position.y + (bone -> target-> transform -> velocity.y * distancec2d(proj -> transform -> position, bone -> target -> transform -> position) / bone -> movementSpeed);

            double normal_x = target_x - proj->transform->position.x;
            double normal_y = target_y - proj->transform->position.y;

            double theta = atan2(normal_y, normal_x);
            Coord2d v = {cos(theta), sin(theta)};

            proj->transform->velocity = { bone -> movementSpeed * v.x, bone -> movementSpeed * v.y};
            proj->transform->position = proj->transform->position + Coord2d{16 * v.x, 16 * v.y};
            bone -> lifetime_timer = time_timer_start(bone -> lifetime);
        };

        e.c.on_death   = [](Entity* e, Component* c){
            Coord2d pos = e -> transform -> position;
            Coord2i chunk{  (int) floor(pos.x / 256.0),
                            (int) floor(pos.y / 256.0)};
            Coord2i tile{(int) (pos.x - (chunk.x * 256)) / 16,
                         (int) (pos.y - (chunk.y * 256)) / 16};

            Chunk* chunkptr = world_get_chunk(chunk);

            //Entity did not die offscreen
            if(chunkptr != nullptr){
                chunkptr -> overlay_tiles[ (tile.y * 16) + tile.x ] = 27;
                world_chunk_refresh_metatextures(e -> transform -> map, chunkptr);
            }

            ((Enemy*)c)->c.on_death(e, c);
        };
    }
}Skeleton;

#endif