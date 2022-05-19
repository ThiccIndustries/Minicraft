/*
 * Created by MajesticWaffle on 5/19/22.
 * Copyright (c) 2022 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */
#ifndef ENEMY
#define ENEMY

#include "geode.h"
#include "player.cpp"

#define COMP_ENEMY 4

typedef struct Enemy{
    Component c;

    double  movementSpeed;
    uint    attack_range;
    uint    follow_range;
    uint    damage;
    double  attack_time;
    Timer*  attack_timer = nullptr;
    Entity* target = nullptr;

    Enemy() {
        /*e.frame_count = 4;
        e.frame_order = new uint[]{0, 1, 0, 2};
        e.col_bounds        = {{5,10}, {11, 15}};
        e.hit_bounds        = {{2, 0}, {14, 15}};
        e.animation_rate    = TIME_TPS;
        e.type = ENT_ENEMY;
        e.health  = 10;*/
        damage = 2;
        attack_time = TIME_TPS;
        c.type = COMP_ENEMY;

        c.on_tick  = [](Entity* e, Component* c){
            Enemy* enemy = (Enemy*)c;

            //Reset target if beyond target distance
            if (enemy->target != nullptr) {
                e->transform->velocity = {0,0};

                double targetDist = distancec2d(enemy->target -> transform ->position, e -> transform -> position);

                //Target is beyond follow distance
                if(targetDist > enemy->follow_range || enemy -> target -> health <= 0) {
                    enemy ->target = nullptr;
                    return; //Wait next tick
                }

                //Enemy should move
                double delta_x = (enemy -> target -> transform -> position.x) - e -> transform -> position.x;
                double delta_y = (enemy-> target -> transform -> position.y) - e -> transform -> position.y;

                if(std::abs(delta_x) > std::abs(delta_y))
                    e -> transform -> direction = delta_x > 0 ? DIRECTION_EAST : DIRECTION_WEST;
                else
                    e -> transform -> direction = delta_y > 0 ? DIRECTION_SOUTH : DIRECTION_NORTH;

                double theta_radians = atan2(delta_y, delta_x);
                e -> transform -> velocity = {enemy -> movementSpeed * cos(theta_radians), enemy -> movementSpeed * sin(theta_radians)};

            } else {
                for (int i = 0; i < g_entity_highest_id; i++) {
                    //Skip self
                    if (i == e -> id)
                        continue;

                    if(g_entity_registry[i] == nullptr)
                        continue;

                    //Found new target
                    if (entity_contains_component(g_entity_registry[i], COMP_PLAYER) && distancec2d(g_entity_registry[i]->transform->position, e->transform->position) <= enemy->follow_range && g_entity_registry[i] -> health > 0) {
                        enemy->target = g_entity_registry[i];
                        break;
                    }
                }
            }
        };
        c.on_death = [](Entity* e, Component* c){
            entity_delete(e -> id);
        };
    }
}Enemy;

#endif