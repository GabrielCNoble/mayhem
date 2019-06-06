#ifndef PLA_COMMON_H
#define PLA_COMMON_H

//#include "phy_common.h"
//#include "r_common.h"
#include "ent.h"

struct player_handle_t
{
    unsigned int player_index;
};

#define INVALID_PLAYER_INDEX 0xffffffff
#define PLAYER_HANDLE(player_index) (struct player_handle_t){player_index}
#define INVALID_PLAYER_HANDLE PLAYER_HANDLE(INVALID_PLAYER_INDEX)

struct player_t
{

    float pitch;
    float yaw;
    vec3_t position;
    vec3_t camera_position;
    float camera_bob;

    struct entity_handle_t entity;
//    struct collider_handle_t collider;

    char *name;
};

#endif // PLA_COMMON_H
