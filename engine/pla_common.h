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
    int remote;
    char *name;
};


/* used to map the player_movement_buffer_t to the correct player... */
struct player_handle_mapping_t
{
    struct player_handle_t client_player_handle;    /* player owned by the client */
    struct player_handle_t server_player_handle;    /* player owned by the server */
};

struct player_movement_buffer_t
{
    struct player_handle_mapping_t player_mapping;
    vec3_t position;
    vec3_t velocity;
};




#endif // PLA_COMMON_H
