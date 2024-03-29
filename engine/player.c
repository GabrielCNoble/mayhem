#include "player.h"
#include "../common/containers/stack_list.h"
#include "input.h"
#include "r_view.h"
#include "r_dbg.h"
#include "console.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>


stack_list_t players;
struct player_handle_t active_player = INVALID_PLAYER_HANDLE;

#ifdef __cplusplus
extern "C"
{
#endif

int32_t player_Init()
{
    players.init(sizeof(struct player_t), 32);
    return 0;
}

void player_Shutdown()
{

}

__declspec(dllexport) __stdcall struct player_handle_t player_CreatePlayer(char *name, vec3_t position, vec3_t camera_position, int remote)
{
//    int player_index = -1;

    struct player_handle_t player_handle = INVALID_PLAYER_HANDLE;
    struct player_t *player;
    struct entity_t *player_entity;
    struct transform_component_t *transform_component;
    struct physics_component_t *physics_component;
    struct player_collider_t *collider;

    player_handle.player_index = players.add(NULL);

    player = (struct player_t *)players.get(player_handle.player_index);

    if(player)
    {
        player->name = strdup(name);
        player->position = position;
        player->camera_position = camera_position;
        player->pitch = 0.0;
        player->yaw = 0.0;
        player->camera_bob = 0.0;
        player->remote = remote;


        player->entity = ent_CreateEntityInstance();
        player_entity = ent_GetEntityPointer(player->entity);

        ent_AddComponent(player->entity, ENT_COMPONENT_TYPE_TRANSFORM);
        ent_AddPhysicsComponent(player->entity, PHY_COLLIDER_TYPE_PLAYER);
        ent_AddNetComponent(player->entity);
//        ent_AddComponent(player->entity, ENT_COMPONENT_TYPE_PHYSICS);

        transform_component = (struct transform_component_t *)ent_GetComponentPointer(player_entity->components[ENT_COMPONENT_TYPE_TRANSFORM]);
        physics_component = (struct physics_component_t *)ent_GetComponentPointer(player_entity->components[ENT_COMPONENT_TYPE_PHYSICS]);

        transform_component->position = position;
        collider = (struct player_collider_t *)phy_GetColliderPointer(physics_component->collider);

        collider->height = 1.0;
        collider->radius = 0.2;
        collider->base.position = position;
    }

    return player_handle;
}

__declspec(dllexport) __stdcall struct player_handle_t player_CreateRemotePlayer(char *name, vec3_t position)
{
    return player_CreatePlayer(name, position, vec3_t(0.0, 0.0, 0.0), 1);
}

__declspec(dllexport) __stdcall struct player_t *player_GetPlayerPointer(struct player_handle_t player_handle)
{
    struct player_t *player;
    player = (struct player_t *)players.get(player_handle.player_index);
    return player;
}

__declspec(dllexport) __stdcall vec3_t player_GetPlayerPosition(struct player_handle_t player_handle)
{
    struct player_t *player;
    player = player_GetPlayerPointer(player_handle);

    if(player)
    {
        return ent_GetEntityPosition(player->entity);
    }

    return vec3_t(0.0, 0.0, 0.0);
}

__declspec(dllexport) __stdcall vec3_t player_GetPlayerLinearVelocity(struct player_handle_t player_handle)
{
    struct player_t *player;
    struct entity_t *entity;
    struct physics_component_t *physics_component;
    struct player_collider_t *player_collider;

    player = player_GetPlayerPointer(player_handle);

    if(player)
    {
        entity = ent_GetEntityPointer(player->entity);
        physics_component = (struct physics_component_t *)ent_GetComponentPointer(entity->components[ENT_COMPONENT_TYPE_PHYSICS]);
        player_collider = phy_GetPlayerColliderPointer(physics_component->collider);
        return player_collider->linear_velocity;
    }

    return vec3_t(0.0, 0.0, 0.0);
}

void player_UpdatePlayers()
{
    player_UpdateActivePlayer();
}

void player_PostUpdatePlayers()
{
    player_PostUpdateActivePlayer();
}

#define CAMERA_BOB_INCREMENT 0.25

//#define SPECTATE

void player_UpdateActivePlayer()
{
    struct player_t *player;
    struct view_t *view;
    vec2_t mouse_delta;
    mat3_t pitch_matrix;
    mat3_t yaw_matrix;
    struct entity_t *player_entity;
    struct physics_component_t *physics_component;

    static collider_handle_t collider = PHY_INVALID_COLLIDER_HANDLE;
    struct player_collider_t *player_collider;

    static int counter = 0;


    vec3_t translation;

    float camera_bob;

    if(active_player.player_index != INVALID_PLAYER_INDEX)
    {
        player = (struct player_t *)players.get(active_player.player_index);

        if(player)
        {
            translation = vec3_t(0.0, 0.0, 0.0);
            mouse_delta = in_GetMouseDelta();

            player->pitch += mouse_delta.y * 0.15;

            if(player->pitch > 0.5) player->pitch = 0.5;
            else if(player->pitch < -0.5) player->pitch = -0.5;

            player->yaw -= mouse_delta.x * 0.15;

            pitch_matrix = rotate_x(player->pitch);
            yaw_matrix = rotate_y(player->yaw);

            view = r_GetActiveView();

            player_entity = ent_GetEntityPointer(player->entity);
            physics_component = (struct physics_component_t *)ent_GetComponentPointer(player_entity->components[ENT_COMPONENT_TYPE_PHYSICS]);

            view->orientation = pitch_matrix * yaw_matrix;

            camera_bob = 0.0;

            if(in_GetKeyStatus(SDL_SCANCODE_W) & KEY_STATUS_PRESSED)
            {
                translation.z += -1.0;
                camera_bob = player->camera_bob + CAMERA_BOB_INCREMENT;
            }

            if(in_GetKeyStatus(SDL_SCANCODE_S) & KEY_STATUS_PRESSED)
            {
                translation.z += 1.0;
                camera_bob = player->camera_bob + CAMERA_BOB_INCREMENT;
            }



            if(in_GetKeyStatus(SDL_SCANCODE_A) & KEY_STATUS_PRESSED)
            {
                translation.x += -1.0;
                camera_bob = player->camera_bob + CAMERA_BOB_INCREMENT;
            }

            if(in_GetKeyStatus(SDL_SCANCODE_D) & KEY_STATUS_PRESSED)
            {
                translation.x += 1.0;
                camera_bob = player->camera_bob + CAMERA_BOB_INCREMENT;
            }

            if(in_GetKeyStatus(SDL_SCANCODE_E) & KEY_STATUS_JUST_PRESSED)
            {
//                struct collider_handle_t projectile;
//                struct projectile_collider_t *projectile_ptr;
//
//                projectile = phy_CreateCollider(PHY_COLLIDER_TYPE_PROJECTILE);
//                projectile_ptr = phy_GetProjectileColliderPointer(projectile);
//
//                projectile_ptr->radius = 0.2;
//                projectile_ptr->linear_velocity = vec3_t(0.0, 0.0, 0.0);
//                projectile_ptr->base.position = vec3_t(0.0, 0.6, 1.0);
            }

            player->camera_bob = camera_bob;


            #ifdef SPECTATE
                translation = normalize(translation);

                if(in_GetMouseStatus() & MOUSE_STATUS_MIDDLE_BUTTON_PRESSED)
                {
                    if(in_GetKeyStatus(SDL_SCANCODE_LCTRL) & KEY_STATUS_PRESSED)
                    {
                        translation.y = -1.0;
                    }

                    if(in_GetKeyStatus(SDL_SCANCODE_SPACE) & KEY_STATUS_PRESSED)
                    {
                        translation.y = 1.0;
                    }
                    view->position += (translation * yaw_matrix) * 0.16;
                }
                else
            #else
                translation = (normalize(translation) * yaw_matrix);
            #endif
            {
                if(in_GetKeyStatus(SDL_SCANCODE_SPACE) & KEY_STATUS_JUST_PRESSED)
                {
                    phy_Jump(physics_component->collider);
                }

                phy_Move(physics_component->collider, translation * 0.016);
            }

            #define FIRE_DELAY 8

            if(counter < FIRE_DELAY)
            {
                counter++;
            }

            if(in_GetMouseStatus() & MOUSE_STATUS_LEFT_BUTTON_PRESSED)
            {
//                if(counter >= FIRE_DELAY)
//                {
//                    struct collider_handle_t projectile;
//                    struct projectile_collider_t *projectile_ptr;
//
//                    projectile = phy_CreateProjectileCollider(view->position, 0.14);
//                    projectile_ptr = phy_GetProjectileColliderPointer(projectile);
//                    projectile_ptr->linear_velocity = -view->orientation[2] * 4.3;
//                    projectile_ptr->overbounce = 2.0;
//                    projectile_ptr->flags |= PHY_PROJECTILE_COLLIDER_FLAG_IGNORE_GRAVITY;
//                    counter = 0;
//                }
            }
        }
    }
}

void player_PostUpdateActivePlayer()
{
    struct player_t *player;
    struct player_collider_t *collider;
    struct entity_t *player_entity;
    struct physics_component_t *physics_component;
    struct transform_component_t *transform_component;
    struct view_t *view;
    vec2_t mouse_delta;
    mat3_t pitch_matrix;
    mat3_t yaw_matrix;

    vec3_t translation;

    if(active_player.player_index != INVALID_PLAYER_INDEX)
    {
        player = (struct player_t *)players.get(active_player.player_index);

        if(player)
        {
//            collider = (struct player_collider_t *)phy_GetColliderPointer(player->collider);
            view = r_GetActiveView();
            player_entity = ent_GetEntityPointer(player->entity);
            transform_component = (struct transform_component_t *)ent_GetComponentPointer(player_entity->components[ENT_COMPONENT_TYPE_TRANSFORM]);
            physics_component = (struct physics_component_t *)ent_GetComponentPointer(player_entity->components[ENT_COMPONENT_TYPE_PHYSICS]);
            collider = (struct player_collider_t *)phy_GetColliderPointer(physics_component->collider);

//            con_Printf("[%f %f %f]", transform_component->position[0], transform_component->position[1], transform_component->position[2]);
            #ifndef SPECTATE
                view->position = transform_component->position + player->camera_position;
            #endif
//            r_DrawCapsule(transform_component->position, collider->height, collider->radius);
        }
    }
}

__declspec(dllexport) __stdcall void player_SetActivePlayer(struct player_handle_t player)
{
    active_player = player;
}

#ifdef __cplusplus
}
#endif



