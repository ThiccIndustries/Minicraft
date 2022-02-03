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
        e1->e.transform.velocity = {0,0};

        double targetDist = distancec2d(e1->target->transform.position, e1->e.transform.position);

        //Target is beyond follow distance
        if(targetDist > e1->follow_range || e1 -> target->health <= 0) {
            e1->target = nullptr;
            return; //Wait next tick
        }

        //Enemy should move
        double delta_x = (e1->target->transform.position.x) - e1 -> e.transform.position.x;
        double delta_y = (e1->target->transform.position.y) - e1 -> e.transform.position.y;

        if(std::abs(delta_x) > std::abs(delta_y))
            e -> direction = delta_x > 0 ? DIRECTION_EAST : DIRECTION_WEST;
        else
            e -> direction = delta_y > 0 ? DIRECTION_SOUTH : DIRECTION_NORTH;

        double theta_radians = atan2(delta_y, delta_x);
        e1 -> e.transform.velocity = {e1 -> movementSpeed * cos(theta_radians), e1 -> movementSpeed * sin(theta_radians)};

    } else {
        for (int i = 0; i < g_entity_highest_id; i++) {
            //Skip self
            if (i == e1->e.id)
                continue;

            if(g_entity_registry[i] == nullptr)
                continue;
            //Found new target
            int targettype = ENT_PLAYER;
            if (g_entity_registry[i] -> type == targettype && distancec2d(g_entity_registry[i]->transform.position, e1->e.transform.position) <= e1->follow_range && g_entity_registry[i] -> health > 0) {
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

    double targetDist = distancec2d(e1->target->transform.position, e1->e.transform.position);

    if(targetDist < e1 -> attack_range){
        if(e1 -> attack_timer == nullptr || time_timer_finished(e1 -> attack_timer)) {
            e1->attack_timer = time_timer_start(e1->attack_time);
            Entity *hit = entity_hit((Entity *) e1, e1->e.transform.velocity);
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
    proj -> e.transform.position = e1 -> e.transform.position;
    proj -> target = e1 -> target;
    proj -> movementSpeed = (4.0 * 16) / TIME_TPS;

    //Recenter coordinate space

    double target_x = proj -> target->transform.position.x + (proj -> target -> transform.velocity.x * distancec2d(proj -> e.transform.position, proj -> target -> transform.position) / proj -> movementSpeed);
    double target_y = proj -> target->transform.position.y + (proj -> target -> transform.velocity.y * distancec2d(proj -> e.transform.position, proj -> target -> transform.position) / proj -> movementSpeed);

    double normal_x = target_x - proj -> e.transform.position.x;
    double normal_y = target_y - proj -> e.transform.position.y;

    double theta = atan2(normal_y, normal_x);
    Coord2d v = {cos(theta), sin(theta)};

    proj -> e.transform.velocity = { proj -> movementSpeed * v.x, proj -> movementSpeed * v.y};
    proj -> e.transform.position = proj -> e.transform.position + Coord2d{16 * v.x, 16 * v.y};
    proj -> lifetime_timer = time_timer_start(proj -> lifetime);
}

void entity_tick_bone(Entity* e){
    Entity_Bone* e1 = (Entity_Bone*)e;

    Entity* hit = entity_hit((Entity*)e1, ((Entity*)e1) -> transform.velocity);
    if( hit != nullptr){
        entity_damage(hit, 1);
        entity_death_bone(e);
        return;
    }

    //Check for wall hits
    Coord2d check_position = entity_collision(e, e->transform.velocity);

    //Bone hit a wall
    if(check_position != e->transform.velocity){
        entity_death_bone(e);
        return;
    }

    if(time_timer_finished( e1 -> lifetime_timer )) {
        entity_death_bone(e);
        return; //TODO: Redundant? I guess if I ever add a new condition it would prevent a crash in case i forget.
    }
}

void entity_death_player(Entity* e){
    Entity_Player* player = (Entity_Player*)e;
    e -> health = -1;

    int atlas_index = e -> atlas_index + 27;
    e -> transform.velocity = {0, 0};
    e -> atlas_index = atlas_index;
    e -> col_bounds = {};
    e -> hit_bounds = {};
    e -> animation_rate = 9999;
    player -> run_speed = 0;
    player -> walk_speed = 0;
    
    time_callback_start(TIME_TPS * 5, [](void* v){
        Entity* e = (Entity*)v;
        error("Game Over.", "Player died on tick: " + std::to_string(g_time -> tick));
        entity_delete(e -> id);
        g_time -> paused = true;
    }, e);
}

void entity_death_enemy(Entity* e){
    entity_delete(e -> id);
}

void entity_death_skeleton(Entity* e){
    Coord2d pos = e -> transform.position;
    Coord2i chunk{  (int) floor(pos.x / 256.0),
                    (int) floor(pos.y / 256.0)};
    Coord2i tile{(int) (pos.x - (chunk.x * 256)) / 16,
                 (int) (pos.y - (chunk.y * 256)) / 16};

    Chunk* chunkptr = world_get_chunk(chunk);
    
    //Entity did not die offscreen
    if(chunkptr != nullptr){
        chunkptr -> overlay_tiles[ (tile.y * 16) + tile.x ] = 27;
        world_chunk_refresh_metatextures(chunkptr);
    }

    entity_death_enemy(e);
}

void entity_death_zombie(Entity* e){
    Coord2d pos = e -> transform.position;
    Coord2i chunk{  (int) floor(pos.x / 256.0),
                    (int) floor(pos.y / 256.0)};
    Coord2i tile{(int) (pos.x - (chunk.x * 256)) / 16,
                 (int) (pos.y - (chunk.y * 256)) / 16};

    Chunk* chunkptr = world_get_chunk(chunk);
    
    //Entity did not die offscreen
    if(chunkptr != nullptr){
        chunkptr -> overlay_tiles[ (tile.y * 16) + tile.x ] = 27;
        world_chunk_refresh_metatextures(chunkptr);
    }


    entity_death_enemy(e);
}

void entity_death_bone(Entity* e){
    e -> health = -1;
    Entity_Bone* eb = (Entity_Bone*)e;

    //Use tick to "randomly" assign sprite
    int atlas_index = (g_time -> tick % 2 == 0) ? 37 : 46;
    e -> transform.velocity = {0, 0};
    e -> atlas_index = atlas_index;
    e -> direction = DIRECTION_SOUTH;
    e -> col_bounds = {};
    e -> hit_bounds = {};
    e -> animation_rate = 9999;

    time_callback_start(TIME_TPS / 2, [](void* v){
        Entity* e = (Entity*)v;
        entity_delete(e -> id);
    }, e);

}
