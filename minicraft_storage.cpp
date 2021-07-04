/*
 * Created by MajesticWaffle on 4/29/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */

#include "minicraft.h"

int storage_add_item(Storage* storage, uint item_id, uint item_count){
    //Find valid slot
    for(int i = 0; i < PLAYER_INVENTORY_SIZE; ++i){
        //existing stack found
        if(storage -> item_v[i] == item_id && storage -> item_c[i] < PLAYER_INVENTORY_STACK_SIZE){
            int takenitems = PLAYER_INVENTORY_STACK_SIZE - storage -> item_c[i];
            storage -> item_c[i] += takenitems;

            //Must split stack to fit
            if(takenitems < item_count){
                storage_add_item(storage, item_id, item_count - takenitems);
                return 0;
            }

            return 0;
        }

        if(storage -> item_v[i] == 0){
            storage -> item_v[i] = item_id;

            //Must split stack to fit
            if(item_count > PLAYER_INVENTORY_STACK_SIZE){
                storage -> item_c[i] = PLAYER_INVENTORY_STACK_SIZE;
                storage_add_item(storage, item_id, item_count - PLAYER_INVENTORY_STACK_SIZE);
                return 0;
            }
            return 0;
        }
    }
    return 1;
}