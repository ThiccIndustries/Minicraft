/*
 * Created by MajesticWaffle on 4/29/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

Entity* EntityRegistry[ENTITY_MAX];

Entity* entity_create(Entity* entity){

    for(int i = 0; i < ENTITY_MAX; ++i){
        if(EntityRegistry[i] == nullptr){
            entity -> id = i;
            EntityRegistry[i] = entity;
            break;
        }
    }

    return entity;
}

//Safely delete Entity pointer while also cleaning entity_registry
void entity_delete(uint id){

    delete EntityRegistry[id];
    EntityRegistry[id] = nullptr;
}

