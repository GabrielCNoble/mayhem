#ifndef PLA_COMMON_H
#define PLA_COMMON_H

#include "phy_common.h"
#include "r_common.h"
#include "handle.h"

struct player_t
{

    float pitch;
    float yaw;
    vec3_t position;
    vec3_t camera_position;
    float camera_bob;

    struct collider_handle_t collider;

    char *name;
};


struct player_view_t
{
    struct view_t view;
    struct player_handle_t player;
};

#endif // PLA_COMMON_H
