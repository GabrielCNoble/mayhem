#ifndef PLAYER_H
#define PLAYER_H

#include "../common/gmath/vector.h"
#include "physics.h"
#include "r_view.h"
#include "pla_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

int32_t player_Init();

void player_Shutdown();

struct player_view_t *player_CreateView();

struct player_handle_t player_CreatePlayer(char *name, vec3_t position, vec3_t camera_position, int type);

struct player_handle_t player_CreateRemotePlayer(char *name, vec3_t position);

struct player_t *player_GetPlayerPointer(struct player_handle_t player_handle);

vec3_t player_GetPlayerPosition(struct player_handle_t player_handle);

vec3_t player_GetPlayerLinearVelocity(struct player_handle_t player_handle);

void player_UpdatePlayers();

void player_PostUpdatePlayers();

void player_UpdateActivePlayer();

void player_PostUpdateActivePlayer();

void player_SetActivePlayer(struct player_handle_t player);

#ifdef __cplusplus
}
#endif

#endif // PLAYER_H
