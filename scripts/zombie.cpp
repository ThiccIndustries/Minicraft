/*
 * Created by MajesticWaffle on 5/19/22.
 * Copyright (c) 2022 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */

//This is a complex example of a component that "extends" from another simpler component

#ifndef ZOMBIE
#define ZOMBIE

#include "geode.h"
#include "enemy.cpp"

#define COMP_ZOMBIE 5

typedef struct Zombie{
    Enemy e;

    Zombie(){
        e.movementSpeed = ((2.0 * 16) / TIME_TPS);
        e.attack_range = 1 * 16;
        e.follow_range = 5 * 16;
        e.attack_time = 32;
        e.c.type = COMP_ZOMBIE;

        e.c.on_tick   = [](Entity* e, Component* c){
            Enemy* enemy = (Enemy*)c;
            enemy -> c.on_tick(e,c);
            if(enemy -> target == nullptr)
                return;

            double targetDist = distancec2d(enemy->target->transform->position, e->transform->position);

            if(targetDist < enemy -> attack_range){
                if(enemy -> attack_timer == nullptr || time_timer_finished(enemy -> attack_timer)) {
                    enemy->attack_timer = time_timer_start(enemy->attack_time);
                    Entity *hit = entity_hit((Collider*)(entity_get_component(e, COMP_COLLIDER)), e->transform);
                    if (hit == enemy -> target) {
                        entity_damage(hit, enemy->damage);
                    }
                }
            }
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
                world_chunk_refresh_metatextures(e->transform->map, chunkptr);
            }

            ((Enemy*)c) -> c.on_death(e,c);
        };
    };
}Zombie;

#endif