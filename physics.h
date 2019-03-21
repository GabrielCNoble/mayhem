#ifndef PHYSICS_H
#define PHYSICS_H



#include "vector.h"
#include "list.h"

struct collision_triangle_t
{
    vec3_t verts[3];
    vec3_t normal;
};


struct bvh_node_t
{
    struct bvh_node_t *left;
    struct bvh_node_t *right;
    struct bvh_node_t *parent;

    vec3_t max;
    vec3_t min;

    struct collision_triangle_t *triangle;
};

struct collision_pair_t
{
    struct base_collider_t *collider_a;
    struct base_collider_t *collider_b;
};

struct collision_triangle_list_t
{
    struct base_collider_t *collider;
    struct list_t triangles;
};


enum PHY_COLLIDER_TYPE
{
    PHY_COLLIDER_TYPE_PLAYER = 0,
    PHY_COLLIDER_TYPE_RIGID,
    PHY_COLLIDER_TYPE_PROJECTILE,
    PHY_COLLIDER_TYPE_NONE,
};



struct collider_handle_t
{
    unsigned type : 3;
    unsigned index : 29;
};

#define INVALID_COLLIDER_INDEX  0x1fffffff
#define COLLIDER_HANDLE(type, index)(struct collider_handle_t){type, index}
#define INVALID_COLLIDER_HANDLE COLLIDER_HANDLE(PHY_COLLIDER_TYPE_NONE, INVALID_COLLIDER_INDEX)


struct base_collider_t
{
    int type;
    vec3_t position;
    vec3_t linear_velocity;
};



enum PHY_PLAYER_COLLIDER_FLAGS
{
    PHY_PLAYER_COLLIDER_FLAG_FLYING = 1,
};

struct player_collider_t
{
    struct base_collider_t base;

    float height;
    float radius;
    int flags;
};



void phy_Init();

void phy_Shutdown();

struct collider_handle_t phy_CreateCollider(int type);

struct base_collider_t *phy_GetColliderPointer(struct collider_handle_t collider);

struct player_collider_t *phy_GetPlayerColliderPointer(struct collider_handle_t collider);

void phy_UpdateColliders();




void phy_Jump(struct collider_handle_t collider);

void phy_Move(struct collider_handle_t collider, vec3_t acceleration);




void phy_ClearBvh();

void phy_BuildBvh(vec3_t *vertices, int vertice_count);




struct list_t *phy_TraverseBvh(vec3_t &aabb_max, vec3_t &aabb_min);







#endif // PHYSICS_H




