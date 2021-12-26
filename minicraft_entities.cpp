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
    Entity_Player* player = (Entity_Player*)e;

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

        if(std::abs(delta_x) > std::abs(delta_y))
            e -> direction = delta_x > 0 ? DIRECTION_EAST : DIRECTION_WEST;
        else
            e -> direction = delta_y > 0 ? DIRECTION_SOUTH : DIRECTION_NORTH;

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
            int targettype = ENT_PLAYER;
            if (g_entity_registry[i] -> type == targettype && distancec2d(g_entity_registry[i]->position, e1->e.position) <= e1->follow_range) {
                e1->target = g_entity_registry[i];
                break;
            }
        }
    }
}

void entity_tick_zombie(Entity* e){
    entity_tick_enemy(e);

    Entity_Enemy* e1 = (Entity_Enemy*)e;

    if(e1 -> target == nullptr)
        return;

    double targetDist = distancec2d(e1->target->position, e1->e.position);

    if(targetDist < e1 -> attack_range){
        if(e1 -> attack_timer == nullptr || time_timer_finished(e1 -> attack_timer)) {
            e1->attack_timer = time_timer_start(e1->attack_time);
            Entity *hit = entity_hit((Entity *) e1, e1->e.velocity);
            if (hit == e1 -> target) {
                entity_damage(hit, e1->damage);
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

    //Recenter coordinate space

    double target_x = proj -> target->position.x + (proj -> target -> velocity.x * distancec2d(proj -> e.position, proj -> target -> position) / proj -> movementSpeed);
    double target_y = proj -> target->position.y + (proj -> target -> velocity.y * distancec2d(proj -> e.position, proj -> target -> position) / proj -> movementSpeed);

    double normal_x = target_x - proj -> e.position.x;
    double normal_y = target_y - proj -> e.position.y;

    double theta = atan2(normal_y, normal_x);
    Coord2d v = {cos(theta), sin(theta)};

    proj -> e.velocity = { proj -> movementSpeed * v.x, proj -> movementSpeed * v.y};
    proj -> e.position = proj -> e.position + Coord2d{16 * v.x, 16 * v.y};
    proj -> lifetime_timer = time_timer_start(proj -> lifetime);
}

void entity_tick_bone(Entity* e){
    Entity_Bone* e1 = (Entity_Bone*)e;

    Entity* hit = entity_hit((Entity*)e1, ((Entity*)e1) -> velocity);
    if( hit != nullptr){
        entity_damage(hit, 1);
        entity_death_bone(e);
    }

    //Check for wall hits
    Coord2d check_position = entity_collision(e, e->velocity);

    //Bone hit a wall
    if(check_position != e->velocity){
        entity_death_bone(e);
    }

}

void entity_death_player(Entity* e){
    error("Game Over.", "Player died on tick: " + std::to_string(g_time -> tick));
    entity_delete(e -> id);
    g_time -> paused = true;
}

void entity_death_enemy(Entity* e){
    entity_delete(e -> id);
}

void entity_death_skeleton(Entity* e){
    entity_delete(e -> id);
}

void entity_death_zombie(Entity* e){
    entity_delete(e -> id);
}
void entity_death_bone(Entity* e){
    e -> health = -1;
    Entity_Bone* eb = (Entity_Bone*)e;

    //Use tick to "randomly" assign sprite
    int atlas_index = (g_time -> tick % 2 == 0) ? 34 : 43;
    e -> velocity = {0, 0};
    e -> atlas_index = atlas_index;
    e -> col_bounds = { {0, 0}, {-1, -1} };
    e -> hit_bounds = { {0, 0}, {-1, -1} };
    e -> animation_rate = 9999;

    time_callback_start(16, [](void* v){
        Entity* e = (Entity*)v;
        entity_delete(e -> id);
    }, e);

}
