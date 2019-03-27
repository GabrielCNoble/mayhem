#include "player.h"
#include "stack_list.h"
#include "input.h"
#include "r_view.h"


#include <stdlib.h>
#include <string.h>


stack_list_t players;
int active_player = -1;


void player_Init()
{
    players.init(sizeof(struct player_t), 32);
}

void player_Shutdown()
{

}

int player_CreatePlayer(char *name, vec3_t position)
{
    int player_index = -1;

    struct player_t *player;
    struct player_collider_t *collider;

    player_index = players.add(NULL);

    player = (struct player_t *)players.get(player_index);

    if(player)
    {
        player->name = strdup(name);
        player->position = position;
        player->pitch = 0.0;
        player->yaw = 0.0;
        player->collider = phy_CreateCollider(PHY_COLLIDER_TYPE_PLAYER);

        collider = (struct player_collider_t *)phy_GetColliderPointer(player->collider);

        collider->height = 1.0;
        collider->radius = 0.2;
        collider->base.position = position;
    }

    return player_index;
}

void player_UpdatePlayers()
{
    player_UpdateActivePlayer();
}

void player_PostUpdatePlayers()
{
    player_PostUpdateActivePlayer();
}

void player_UpdateActivePlayer()
{
    struct player_t *player;
    struct view_t *view;
    vec2_t mouse_delta;
    mat3_t pitch_matrix;
    mat3_t yaw_matrix;

    vec3_t translation;

    if(active_player != -1)
    {
        player = (struct player_t *)players.get(active_player);

        if(player)
        {
            translation = vec3_t(0.0, 0.0, 0.0);
            mouse_delta = in_GetMouseDelta();

            player->pitch += mouse_delta.y * 0.15;

            if(player->pitch > 0.5) player->pitch = 0.5;
            else if(player->pitch < -0.5) player->pitch = -0.5;

            player->yaw -= mouse_delta.x * 0.15;

            pitch_matrix = rotate_x(pitch_matrix, player->pitch);
            yaw_matrix = rotate_y(yaw_matrix, player->yaw);

            view = r_GetActiveView();

            view->orientation = pitch_matrix * yaw_matrix;

            if(in_GetKeyStatus(SDL_SCANCODE_W) & KEY_STATUS_PRESSED)
            {
                translation.z += -1.0;
            }

            if(in_GetKeyStatus(SDL_SCANCODE_S) & KEY_STATUS_PRESSED)
            {
                translation.z += 1.0;
            }



            if(in_GetKeyStatus(SDL_SCANCODE_A) & KEY_STATUS_PRESSED)
            {
                translation.x += -1.0;
            }

            if(in_GetKeyStatus(SDL_SCANCODE_D) & KEY_STATUS_PRESSED)
            {
                translation.x += 1.0;
            }


            if(in_GetKeyStatus(SDL_SCANCODE_SPACE) & KEY_STATUS_JUST_PRESSED)
            {
                phy_Jump(player->collider);
            }

            translation = (normalize(translation) * yaw_matrix) * 0.016;

            phy_Move(player->collider, translation);

            //collider = (struct player_collider_t *)phy_GetColliderPointer(player->collider);

            //collider->base.position += (translation * yaw_matrix) * 0.1;
        }
    }
}

void player_PostUpdateActivePlayer()
{
    struct player_t *player;
    struct player_collider_t *collider;
    struct view_t *view;
    vec2_t mouse_delta;
    mat3_t pitch_matrix;
    mat3_t yaw_matrix;

    vec3_t translation;

    if(active_player != -1)
    {
        player = (struct player_t *)players.get(active_player);

        if(player)
        {
            collider = (struct player_collider_t *)phy_GetColliderPointer(player->collider);
            view = r_GetActiveView();

            player->position = collider->base.position;

            view->position = player->position + vec3_t(0.0, 0.3, 0.0);
        }
    }
}

void player_SetActivePlayer(int player_index)
{
    active_player = player_index;
}



