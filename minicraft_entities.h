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
    ENT_PROJ,
    ENT_ZOMBIE,
    ENT_SKELETON,
    ENT_PROJ_BONE
};

//Player
typedef struct Entity_Player{
    Entity e;

    double  movementSpeed;
    uint    health;

    Entity_Player(){
        e.atlas_index       = 0;
        e.spritesheet_size  = {3, 3};
        e.frame_count       = 4;
        e.frame_order       = new uint[]{0, 1, 0, 2};
        e.col_bounds        = {{5,10}, {11, 15}};
        e.hit_bounds        = {{2, 0}, {14, 15}};
        e.animation_rate    = TIME_TPS;
        e.type              = ENT_PLAYER;

        movementSpeed   = ((3.0 * 16) / TIME_TPS);
        health          = 5;
    }
}Entity_Player;

typedef struct Entity_Enemy{
    Entity e;

    double  movementSpeed;
    uint    health;
    uint    attack_range;
    uint    follow_range;
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
    }
}Entity_Enemy;

typedef struct Entity_Projectile{
    Entity e;

    double  movementSpeed;
    double  lifetime;
    Timer*  lifetime_timer;
    Entity* target;

    Entity_Projectile() {
        e.type = ENT_PROJ;
        lifetime_timer  = nullptr;
        target          = nullptr;
    }
} Entity_Projectile;

typedef struct Entity_Zombie{
    Entity_Enemy e;

    Entity_Zombie(){
        e.e.atlas_index = 3;
        e.e.type = ENT_ZOMBIE;
        e.movementSpeed = ((2.5 * 16) / TIME_TPS);
        e.health = 10;
        e.attack_range = 1 * 16;
        e.follow_range = 5 * 16;
        e.attack_time = 32;
    };
}Entity_Zombie;

typedef struct Entity_Skeleton{
    Entity_Enemy e;

    Entity_Skeleton(){
        e.e.atlas_index = 6;
        e.e.type        = ENT_SKELETON;
        e.movementSpeed = ((1.5 * 16) / TIME_TPS);
        e.health        = 10;
        e.attack_range  = 5 * 16;
        e.follow_range  = 5 * 16;
        e.attack_time   = 64;
    }
}Entity_Skeleton;

typedef struct Entity_Projectile_Bone{
    Entity_Projectile e;

    Entity_Projectile_Bone() {
        e.e.atlas_index = 27;
        e.e.spritesheet_size = {1, 2};
        e.e.frame_count = 2;
        e.e.frame_order = new uint[]{0, 1};
        e.e.col_bounds  = {{2, 2}, {9, 9}};
        e.e.hit_bounds  = {{1, 1}, {10, 10}};
        e.e.animation_rate = 16;
        e.e.type = ENT_PROJ_BONE;

        e.movementSpeed   = ((5.0 * 16) / TIME_TPS);
        e.lifetime        = 2 * TIME_TPS;
        e.lifetime_timer  = nullptr;
        e.target          = nullptr;
    }
}Entity_Projectile_Bone;

#endif //MINICRAFT_CB_MINICRAFT_ENTITIES_H
