#ifndef HANDLES_H
#define HANDLES_H


struct collider_handle_t
{
    unsigned type : 3;
    unsigned index : 29;
};

#define INVALID_COLLIDER_INDEX  0x1fffffff
#define COLLIDER_HANDLE(type, index)(struct collider_handle_t){type, index}
#define INVALID_COLLIDER_HANDLE COLLIDER_HANDLE(PHY_COLLIDER_TYPE_NONE, INVALID_COLLIDER_INDEX)




struct player_handle_t
{
    unsigned int player_index;
};

#define INVALID_PLAYER_INDEX 0xffffffff
#define PLAYER_HANDLE(player_index) (struct player_handle_t){player_index}
#define INVALID_PLAYER_HANDLE PLAYER_HANDLE(INVALID_PLAYER_INDEX)




struct collision_shape_handle_t
{
    unsigned type : 3;
    unsigned index : 29;
};

#define COLLISION_SHAPE_HANDLE(type, index)(struct collision_shape_handle_t){type, index}
#define INVALID_COLLISION_SHAPE_INDEX 0x1fffffff
#define INVALID_COLLISION_SHAPE_HANDLE COLLISION_SHAPE_HANDLE(PHY_COLLISION_SHAPE_NONE, INVALID_COLLISION_SHAPE_INDEX)




struct component_handle_t
{
    unsigned type : 4;
    unsigned def : 1;
    unsigned index : 27;
};

#define COMPONENT_HANDLE(type, def, index) (struct component_handle_t){type, def, index}
#define INVALID_COMPONENT_INDEX 0x07ffffff
#define INVALID_COMPONENT_HANDLE COMPONENT_HANDLE(0, 0, INVALID_COMPONENT_INDEX)




struct entity_handle_t
{
    unsigned def : 1;
    unsigned index : 31;
};

#define ENTITY_HANDLE(def, index)(struct entity_handle_t){def, index}
#define INVALID_ENTITY_INDEX 0x7fffffff
#define INVALID_ENTITY_HANDLE ENTITY_HANDLE(1, INVALID_ENTITY_INDEX)

#endif // HANDLES_H
