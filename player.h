#ifndef PLAYER_H
#define PLAYER_H

#include "vector.h"
#include "physics.h"
#include "r_view.h"
#include "pla_common.h"


void player_Init();

void player_Shutdown();

struct player_view_t *player_CreateView();

struct player_handle_t player_CreatePlayer(char *name, vec3_t position, vec3_t camera_position);

struct player_t *player_GetPlayerPointer(struct player_handle_t player_handle);

void player_UpdatePlayers();

void player_PostUpdatePlayers();

void player_UpdateActivePlayer();

void player_PostUpdateActivePlayer();

void player_SetActivePlayer(struct player_handle_t player);


#endif // PLAYER_H
