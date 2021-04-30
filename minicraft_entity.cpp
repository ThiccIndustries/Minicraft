/*
 * Created by MajesticWaffle on 4/29/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

Entity* g_entity_registry[ENTITY_MAX];
uint g_entity_highest_id = 0;

Entity* entity_create(Entity* entity){

    for(int i = 0; i < ENTITY_MAX; ++i){
        if(g_entity_registry[i] == nullptr){
            entity -> id = i;
            g_entity_registry[i] = entity;

            if(i > g_entity_highest_id)
                g_entity_highest_id = i;

            break;
        }
    }

    return entity;
}

//Safely delete Entity pointer while also cleaning entity_registry
void entity_delete(uint id){

    //TODO: this only kinda works
    if(id == g_entity_highest_id)
        g_entity_highest_id--;

    delete g_entity_registry[id];
    g_entity_registry[id] = nullptr;
}

void entity_tick(){
    for(int i = 0; i <= g_entity_highest_id; ++i){

        switch(g_entity_registry[i] -> type){
            case ENT_GENERIC:
                break;
            case ENT_PLAYER:
                ((Entity_Player*)g_entity_registry[i]) -> camera.position = g_entity_registry[i] -> position;
                break;
        }
    }
}

