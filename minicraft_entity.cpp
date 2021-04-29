/*
 * Created by MajesticWaffle on 4/29/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

Entity* entity_registry[ENTITY_MAX];

void entity_register_entity(Entity* entity){
    for(int i = 0; i < ENTITY_MAX; ++i){
        std::cout << i << std::endl;
        if(entity_registry[i] == nullptr){
            entity -> id = i;
            entity_registry[i] = entity;
            break;
        }
    }
}

