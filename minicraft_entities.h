/*
 * Created by MajesticWaffle on 7/18/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */

#ifndef MINICRAFT_CB_MINICRAFT_ENTITIES_H
#define MINICRAFT_CB_MINICRAFT_ENTITIES_H

#include "geode.h"

enum EntityType{
    ENT_GENERIC,
    ENT_PLAYER,
    ENT_ENEMY,
    ENT_ZOMBIE,
    ENT_SKELETON,
    ENT_BONE
};

void entity_tick_player(Entity* e);
void entity_tick_enemy(Entity* e);
void entity_tick_bone(Entity* e);
void entity_tick_skeleton(Entity* e);
void entity_tick_zombie(Entity* e);

void entity_death_player(Entity* e);
void entity_death_enemy(Entity* e);
void entity_death_bone(Entity* e);
void entity_death_skeleton(Entity* e);
void entity_death_zombie(Entity* e);

//Player
typedef struct Entity_Player{
    Entity e;

    double  walk_speed, run_speed;
    uint    range;
    uint    stamina;

    uint    tmp_debug;
    uint    stamina_delay,  collect_delay,  attack_delay;
    Timer  *stamina_timer, *collect_timer, *attack_timer;

    Entity_Player(){
        e.atlas_index       = 0;
        e.spritesheet_size  = {3, 3};
        e.frame_count       = 4;
        e.frame_order       = new uint[]{0, 1, 0, 2};
        e.col_bounds        = {{5,10}, {11, 15}};
        e.hit_bounds        = {{2, 0}, {14, 15}};
        e.animation_rate    = TIME_TPS;
        e.type              = ENT_PLAYER;
        e.health            = 1000;

        e.tick_func         = &entity_tick_player;
        e.death_func         = &entity_death_player;

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
    }

}Entity_Player;

typedef struct Entity_Enemy{
    Entity e;

    double  movementSpeed;
    uint    attack_range;
    uint    follow_range;
    uint    damage;
    double  attack_time;
    Timer*  attack_timer = nullptr;
    Entity* target = nullptr;

    Entity_Enemy() {
        e.frame_count = 4;
        e.frame_order = new uint[]{0, 1, 0, 2};
        e.col_bounds        = {{5,10}, {11, 15}};
        e.hit_bounds        = {{2, 0}, {14, 15}};
        e.animation_rate    = TIME_TPS;
        e.type = ENT_ENEMY;
        e.health  = 10;
        e.tick_func = &entity_tick_enemy;
        e.death_func = &entity_death_enemy;
        damage = 2;
        attack_time = TIME_TPS;
    }
}Entity_Enemy;

typedef struct Entity_Zombie{
    Entity_Enemy e;

    Entity_Zombie(){
        e.e.atlas_index = 3;
        e.e.spritesheet_size  = {3, 3};
        e.e.type = ENT_ZOMBIE;
        e.e.tick_func   = &entity_tick_zombie;
        e.e.death_func   = &entity_death_zombie;
        e.movementSpeed = ((2.0 * 16) / TIME_TPS);
        e.e.health = 10;
        e.attack_range = 1 * 16;
        e.follow_range = 5 * 16;
        e.attack_time = 32;
    };
}Entity_Zombie;

typedef struct Entity_Skeleton{
    Entity_Enemy e;

    Entity_Skeleton(){
        e.e.atlas_index = 6;
        e.e.spritesheet_size  = {3, 3};
        e.e.type        = ENT_SKELETON;
        e.e.tick_func   = &entity_tick_skeleton;
        e.e.death_func   = &entity_death_skeleton;
        e.movementSpeed = ((1.5 * 16) / TIME_TPS);
        e.e.health        = 10;
        e.attack_range  = 5 * 16;
        e.follow_range  = 5 * 16;
        e.attack_time   = 64;
    }
}Entity_Skeleton;

typedef struct Entity_Bone{
    Entity e;

    double  movementSpeed;
    double  lifetime;
    Timer*  lifetime_timer;
    Entity* target;

    Entity_Bone() {
        e.atlas_index = 33;
        e.spritesheet_size = {1, 2};
        e.frame_count = 2;
        e.frame_order = new uint[]{0, 1};
        e.col_bounds  = {{2, 2}, {9, 9}};
        e.hit_bounds  = {{1, 1}, {10, 10}};
        e.animation_rate = 16;
        e.type = ENT_BONE;
        e.tick_func = &entity_tick_bone;
        e.death_func = &entity_death_bone;
        e.health = 999;

        movementSpeed   = ((5.0 * 16) / TIME_TPS);
        lifetime        = 2 * TIME_TPS;
        lifetime_timer  = nullptr;
        target          = nullptr;
    }
}Entity_Bone;


#endif //MINICRAFT_CB_MINICRAFT_ENTITIES_H
