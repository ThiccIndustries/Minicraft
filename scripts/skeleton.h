/*
 * Created by MajesticWaffle on 5/19/22.
 * Copyright (c) 2022 Thicc Industries. All rights reserved.
 * This software is under the: Thicc-Industries-Do-What-You-Want-I-Dont-Care License.
 */
#ifndef SKELETON
#define SKELETON
#define COMP_SKELETON 6

typedef struct Skeleton{
    Enemy e;

    Skeleton(){

        e.c.on_create = [](Entity* e, Component* c){
            enemy_on_create(e, c);
            auto enemy = (Enemy*)c;
            auto e_renderer = entity_get_component<Renderer>(e);
            auto e_col = entity_get_component<Collider>(e);
            enemy->attack_range = 5 * 16;
            enemy->follow_range = 5 * 16;
            enemy->attack_time = TIME_TPS;
            enemy->movementSpeed = ((1.5 * 16) / TIME_TPS);
            e->health = 10;
            e_renderer->atlas_texture =
                    texture_load(get_resource_path(g_game_path, "resources/entities/skeleton.bmp"), TEXTURE_MULTIPLE,
                                 16);
            e_renderer->sheet_type = SHEET_UDH;
        };

        e.c.on_tick   = [](Entity* e, Component* c){
            Enemy* enemy = (Enemy*)c;
            enemy_on_tick(e, c);

            //Attack
            if(!(enemy -> attack_timer == nullptr || time_timer_finished(enemy -> attack_timer)))
                return;

            if(enemy -> target == nullptr)
                return;

            enemy -> attack_timer = time_timer_start(enemy -> attack_time);
            Entity* proj = entity_create();
            auto collider = entity_add_component<Collider>(proj);
            auto renderer = entity_add_component<Renderer>(proj);
            auto bone = entity_add_component<Bone>(proj);

            proj->transform->position = e->transform->position;
            proj->transform->map = e->transform->map;
            bone -> target = enemy -> target;
            bone -> movementSpeed = (4.0 * 16) / TIME_TPS;

            //Recenter coordinate space

            double target_x = bone -> target-> transform -> position.x + (bone -> target-> transform -> velocity.x * distancec2d(proj -> transform -> position, bone -> target -> transform -> position) / bone -> movementSpeed);
            double target_y = bone -> target-> transform -> position.y + (bone -> target-> transform -> velocity.y * distancec2d(proj -> transform -> position, bone -> target -> transform -> position) / bone -> movementSpeed);

            double normal_x = target_x - proj->transform->position.x;
            double normal_y = target_y - proj->transform->position.y;

            double theta = atan2(normal_y, normal_x);
            Coord2d v = {cos(theta), sin(theta)};

            proj->transform->velocity = { bone -> movementSpeed * v.x, bone -> movementSpeed * v.y};
            proj->transform->position = proj->transform->position + Coord2d{32 * v.x, 32 * v.y};
            bone -> lifetime_timer = time_timer_start(bone -> lifetime);
        };

        e.c.on_death   = [](Entity* e, Component* c){
            entity_delete(e->id);
        };
    }
}Skeleton;

#endif