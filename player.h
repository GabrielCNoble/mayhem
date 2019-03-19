#ifndef PLAYER_H
#define PLAYER_H

#include "vector.h"
#include "physics.h"


struct player_t
{

    float pitch;
    float yaw;
    vec3_t position;

    struct collider_handle_t collider;

    char *name;
};


void player_Init();

void player_Shutdown();

int player_CreatePlayer(char *name, vec3_t position);

void player_UpdatePlayers();

void player_PostUpdatePlayers();

void player_UpdateActivePlayer();

void player_PostUpdateActivePlayer();

void player_SetActivePlayer(int player_index);


#endif // PLAYER_H
