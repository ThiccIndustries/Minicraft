/*
 * Created by MajesticWaffle on 8/18/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */

#include "minicraft.h"

//Entity tick func pointers

void entity_tick_callback(Entity* e){
}

void entity_tick_player(Entity* e){

}
void entity_tick_enemy(Entity* e){
    Entity_Enemy* e1 = (Entity_Enemy*)e;

    //Reset target if beyond target distance
    if (e1->target != nullptr) {
        e1->e.velocity = {0,0};

        double targetDist = distancec2d(e1->target->position, e1->e.position);
        //Target is beyond follow distance
        if(targetDist > e1->follow_range) {
            e1->target = nullptr;
            return; //Wait next tick
        }

        //Enemy should move
        double delta_x = (e1->target->position.x) - e1 -> e.position.x;
        double delta_y = (e1->target->position.y) - e1 -> e.position.y;

        double theta_radians = atan2(delta_y, delta_x);
        e1 -> e.velocity = {e1 -> movementSpeed * cos(theta_radians), e1 -> movementSpeed * sin(theta_radians)};

    } else {

        for (int i = 0; i < g_entity_highest_id; i++) {
            //Skip self
            if (i == e1->e.id)
                continue;

            if(g_entity_registry[i] == nullptr)
                continue;
            //Found new target
            if (g_entity_registry[i] -> type == ENT_PLAYER && distancec2d(g_entity_registry[i]->position, e1->e.position) <= e1->follow_range) {
                e1->target = g_entity_registry[i];
                break;
            }
        }
    }
}
void entity_tick_skeleton(Entity* e){
    entity_tick_enemy(e);

    Entity_Enemy* e1 = (Entity_Enemy*)e;

    //Attack
    if(!(e1 -> attack_timer == nullptr || time_timer_finished(e1 -> attack_timer)))
        return;

    if(e1 -> target == nullptr)
        return;

    e1 -> attack_timer = time_timer_start(e1 -> attack_time);
    Entity_Bone* proj = (Entity_Bone*)entity_create((Entity*)new Entity_Bone);
    proj -> e.position = e1 -> e.position;
    proj -> target = e1 -> target;
    proj -> movementSpeed = (4.0 * 16) / TIME_TPS;
    double delta_x = (proj -> target->position.x) - proj -> e.position.x;
    double delta_y = (proj -> target->position.y) - proj -> e.position.y;

    double theta_radians = atan2(delta_y, delta_x);
    Coord2d v = {cos(theta_radians), sin(theta_radians)};

    proj -> e.velocity = { proj -> movementSpeed * v.x, proj -> movementSpeed * v.y};

    proj -> e.position = proj -> e.position + Coord2d{18 * v.x, 10 * v.y};

    proj -> lifetime_timer = time_timer_start(proj -> lifetime);

}

void entity_tick_bone(Entity* e){
    Entity_Bone* e1 = (Entity_Bone*)e;

    Entity* hit = entity_hit((Entity*)e1, ((Entity*)e1) -> velocity);
    if( hit != nullptr && hit -> type == ENT_PLAYER){
        ((Entity_Player*)hit) -> health--;
        entity_delete( ((Entity*)e1) -> id );
    }

}
