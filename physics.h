#ifndef PHYSICS_H
#define PHYSICS_H



#include "vector.h"
#include "list.h"



struct collider_handle_t
{
    unsigned type : 3;
    unsigned index : 29;
};

#define INVALID_COLLIDER_INDEX  0x1fffffff
#define COLLIDER_HANDLE(type, index)(struct collider_handle_t){type, index}
#define INVALID_COLLIDER_HANDLE COLLIDER_HANDLE(PHY_COLLIDER_TYPE_NONE, INVALID_COLLIDER_INDEX)


/*
============================================
============================================
============================================
*/


struct dbvh_node_t
{
    int parent;
    int children[2];

    struct collider_handle_t collider;

    vec3_t max;
    vec3_t min;
};


/*
============================================
============================================
============================================
*/

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

    void *data;
};

struct collision_pair_t
{
    struct collider_handle_t collider_a;
    struct collider_handle_t collider_b;
};

struct contact_point_t
{
    vec3_t position;
    vec3_t normal;
    float penetration;
};

enum PHY_COLLIDER_TYPE
{
    PHY_COLLIDER_TYPE_PLAYER = 0,
    PHY_COLLIDER_TYPE_RIGID,
    PHY_COLLIDER_TYPE_PROJECTILE,
    PHY_COLLIDER_TYPE_PORTAL,
    PHY_COLLIDER_TYPE_STATIC,
    PHY_COLLIDER_TYPE_NONE,
};


struct base_collider_t
{
    int type;
    int dbvh_node;
    vec3_t position;
    vec3_t linear_velocity;
};


/*
============================================
============================================
============================================
*/


struct static_collider_t
{
    struct base_collider_t base;
    struct bvh_node_t *nodes;
    struct list_t vertices;
    int collision_triangles_count;
    struct collision_triangle_t *collision_triangles;
};


/*
============================================
============================================
============================================
*/

enum PHY_PLAYER_COLLIDER_FLAGS
{
    PHY_PLAYER_COLLIDER_FLAG_ON_GROUND = 1,
};

struct player_collider_t
{
    struct base_collider_t base;

    int player_handle;

    float height;
    float radius;
    int flags;
};


/*
============================================
============================================
============================================
*/

struct portal_collider_t
{
    struct base_collider_t base;

//    mat3_t orientation;
//    vec2_t scale;

    int portal_handle;
};


#ifdef __cplusplus
extern "C"
{
#endif

void phy_Init();

void phy_Shutdown();

struct collider_handle_t phy_CreateCollider(int type);

struct collider_handle_t phy_CreateStaticCollider();

void phy_DestroyCollider(struct collider_handle_t collider_handle);

struct base_collider_t *phy_GetColliderPointer(struct collider_handle_t collider);



struct player_collider_t *phy_GetPlayerColliderPointer(struct collider_handle_t collider);

struct portal_collider_t *phy_GetPortalColliderPointer(struct collider_handle_t collider);

struct static_collider_t *phy_GetStaticColliderPointer(struct collider_handle_t collider);

void phy_Step();



/*
============================================
============================================
============================================
*/


void phy_Jump(struct collider_handle_t collider);

void phy_Move(struct collider_handle_t collider, vec3_t acceleration);



/*
============================================
============================================
============================================
*/


int phy_AllocDbvhNode();

void phy_DeallocDbvhNode(int node_index);

struct dbvh_node_t *phy_GetDbvhNodePointer(int node_index);

void phy_GenerateColliderCollisionPairs(struct collider_handle_t collider);

void phy_GenerateCollisionPairs();

void phy_DbvhNodesVolume(int node_a, int node_b, vec3_t *max, vec3_t *min);


/*
============================================
============================================
============================================
*/


void phy_ClearBvh(struct bvh_node_t *node);

struct bvh_node_t *phy_BuildBvhRecursive(struct bvh_node_t *nodes);

//void phy_BuildBvh(vec3_t *vertices, int vertice_count);

void phy_AddStaticTriangles(struct collider_handle_t static_collider, vec3_t *vertices, int count);

void phy_StaticColliderBvh(struct collider_handle_t static_collider);



/*
============================================
============================================
============================================
*/


struct list_t *phy_BoxOnBvh(vec3_t &aabb_max, vec3_t &aabb_min, struct bvh_node_t *nodes);

struct list_t *phy_FindContactPointsPlayerStatic(struct collider_handle_t player_collider, struct collider_handle_t static_collider);

struct list_t *phy_FindContactPointsPlayerPortal(struct collider_handle_t player_collider, struct collider_handle_t portal_collider);




void phy_Collide(struct collider_handle_t collider_a, struct collider_handle_t collider_b);

//void phy_CollidePlayerWorld(struct collider_handle_t player_collider);

void phy_CollidePlayerStatic(struct collider_handle_t player_collider, struct collider_handle_t static_collider);

void phy_CollidePlayerPortal(struct collider_handle_t player_collider, struct collider_handle_t portal_collider);



#ifdef __cplusplus
}
#endif



#endif // PHYSICS_H




