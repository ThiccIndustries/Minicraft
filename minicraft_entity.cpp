/*
 * Created by MajesticWaffle on 4/29/21.
 * Copyright (c) 2021 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */

#include <cmath>
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

    //Recalculate g_entity_highest_id
    if(id == g_entity_highest_id){
        for(uint i = g_entity_highest_id; i >= 0; --i){
            if(g_entity_registry[i] != nullptr) {
                g_entity_highest_id = i;
                break;
            }
        }
    }

    delete g_entity_registry[id];
    g_entity_registry[id] = nullptr;
}

//TODO: test each movement axis separately
void entity_move(Entity* entity, Coord2d delta, bool respect_collisions){

    if(delta.x == 0 && delta.y == 0)
        return;

    //Split axis into two separate movement checks to allow "slipping" on a wall
    //TODO: Was recursion *really* the best way to do this? This feels like a BS APCS test question.
    if(delta.x != 0 && delta.y != 0){
        entity_move(entity, {delta.x, 0}, respect_collisions);
        entity_move(entity, {0, delta.y}, respect_collisions);
        return;
    }

    Coord2d new_delta = delta;

    if(respect_collisions)
        new_delta = entity_collision(entity, delta);

    Coord2d new_ent_pos = entity -> position + new_delta;

    entity -> position = new_ent_pos;
    entity -> move_state = MOVE_ACTIVE;

    if(delta.x > 0)
        entity -> direction = DIRECTION_EAST;
    if(delta.x < 0)
        entity -> direction = DIRECTION_WEST;
    if(delta.y > 0)
        entity -> direction = DIRECTION_SOUTH;
    if(delta.y < 0)
        entity -> direction = DIRECTION_NORTH;
}

void entity_tick(){
    for(int i = 0; i <= g_entity_highest_id; ++i){

        g_entity_registry[i] -> move_state = MOVE_STATIONARY;
        entity_move(g_entity_registry[i], g_entity_registry[i]->velocity, true);

        switch(g_entity_registry[i] -> type){
            case ENT_GENERIC:
                break;
            case ENT_PLAYER:
                if( ((Entity_Player*)g_entity_registry[i]) -> health == 0){
                    error("You were slain.", "Player died on tick: " + std::to_string(g_time -> tick));
                }
                break;
        }
    }
}


Coord2d entity_collision(Entity* entity, Coord2d delta){

    //std::cout << delta.x << " " << delta.y << std::endl;

    Coord2d new_ent_pos = entity -> position + delta;

    //Global coordinates of bounding box, with new desired position
    BoundingBox global_bb = { {entity->bounds.p1.x + new_ent_pos.x, entity->bounds.p1.y + new_ent_pos.y},
                              {entity->bounds.p2.x + new_ent_pos.x, entity->bounds.p2.y + new_ent_pos.y} };

    Coord2i chunk;
    Coord2i tile{ (int)(entity->position.x / 16),
                  (int)(entity->position.y  / 16) };

    Chunk* chunkptr;
    Coord2i rel_tile;

    //Loop though tiles
    for(int x = tile.x - 2; x < tile.x + 2; x++){
        for(int y = tile.y - 2; y < tile.y + 2; y++){

            chunk = { (int)floor((double)x / 16.0),
                      (int)floor((double)y / 16.0) };

            rel_tile = { (int)((double)x - (chunk.x * 16)),
                         (int)((double)y - (chunk.y * 16)) };

            chunkptr = world_get_chunk(chunk);

            //Tile is not solid, no need to check AABBa
            if(!(g_block_registry[ chunkptr->foreground_tiles[ rel_tile.x + (rel_tile.y * 16) ] ]->options & TILE_SOLID))
                continue;

            //TODO: is this even right?
            BoundingBox tile_bb = { {(rel_tile.x * 16.0) + (chunk.x * 256.0)      , (rel_tile.y * 16.0) + (chunk.y * 256.0)},
                                    {(rel_tile.x * 16.0) + (chunk.x * 256.0) + 16 , (rel_tile.y * 16.0) + (chunk.y * 256.0) + 16} };

            //Test AABB vs AABB
            if(entity_AABB(tile_bb, global_bb)){
                double maximum_delta_x = 0, maximum_delta_y = 0;

                if(delta.x > 0) {
                    maximum_delta_x = delta.x - (global_bb.p2.x + 0.001 - tile_bb.p1.x);

                }
                else if(delta.x < 0){
                    maximum_delta_x = delta.x - (global_bb.p1.x - 0.001 - tile_bb.p2.x);
                }

                if(delta.y > 0){
                    maximum_delta_y = delta.y - (global_bb.p2.y + 0.001 - tile_bb.p1.y);
                }
                else if(delta.y < 0){
                    maximum_delta_y = delta.y - (global_bb.p1.y - 0.001 - tile_bb.p2.y);
                }


                return Coord2d{maximum_delta_x, maximum_delta_y};
            }
        }
    }

    //Check all entities
    //TODO: better way?

    for(int i = 0; i <= g_entity_highest_id; ++i){
        Entity* checking_ent = g_entity_registry[i];
        if(checking_ent == entity)
            continue;

        BoundingBox ent_g_bb = {
                {checking_ent->bounds.p1.x + checking_ent->position.x, checking_ent->bounds.p1.y + checking_ent->position.y},
                {checking_ent->bounds.p2.x + checking_ent->position.x, checking_ent->bounds.p2.y + checking_ent->position.y},
        };

        if(entity_AABB(ent_g_bb, global_bb)){
            double maximum_delta_x = 0, maximum_delta_y = 0;

            if(delta.x > 0) {
                maximum_delta_x = delta.x - (global_bb.p2.x + 0.001 - ent_g_bb.p1.x);

            }
            else if(delta.x < 0){
                maximum_delta_x = delta.x - (global_bb.p1.x - 0.001 - ent_g_bb.p2.x);
            }

            if(delta.y > 0){
                maximum_delta_y = delta.y - (global_bb.p2.y + 0.001 - ent_g_bb.p1.y);
            }
            else if(delta.y < 0){
                maximum_delta_y = delta.y - (global_bb.p1.y - 0.001 - ent_g_bb.p2.y);
            }


            return Coord2d{maximum_delta_x, maximum_delta_y};
        }
    }

    return delta;
}

bool entity_AABB(BoundingBox a, BoundingBox b){
    return (a.p1.x <= b.p2.x && a.p2.x >= b.p1.x) && (a.p1.y <= b.p2.y && a.p2.y >= b.p1.y);
}


