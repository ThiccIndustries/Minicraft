/*
 * Created by MajesticWaffle on 4/29/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 */

#include "minicraft.h"

int player_inventory_add_item(Entity_Player* player, uint item_id, uint item_count){

    //Find valid slot
    for(int i = 0; i < PLAYER_INVENTORY_SIZE; ++i){

        //existing stack found
        if(player -> item_v[i] == item_id && player -> item_c[i] < PLAYER_INVENTORY_STACK_SIZE){

            int takenitems = PLAYER_INVENTORY_STACK_SIZE - player -> item_c[i];
            player -> item_c[i] += takenitems;

            //Must split stack to fit
            if(takenitems < item_count){
                player_inventory_add_item(player, item_id, item_count - takenitems);
                return 0;
            }

            return 0;
        }

        if(player -> item_v[i] == 0){

            player -> item_v[i] = item_id;

            //Must split stack to fit
            if(item_count > PLAYER_INVENTORY_STACK_SIZE){
                player -> item_c[i] = PLAYER_INVENTORY_STACK_SIZE;
                player_inventory_add_item(player, item_id, item_count - PLAYER_INVENTORY_STACK_SIZE);
                return 0;
            }

            return 0;
        }
    }

    return 1;
}