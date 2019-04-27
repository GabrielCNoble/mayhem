#ifndef PHY_COMMON_H
#define PHY_COMMON_H


#include "vector.h"
#include "matrix.h"
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
    vec3_t surface_normal;
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

/*
============================================
============================================
============================================
*/

enum PHY_COLLISION_SHAPES
{
    PHY_COLLISION_SHAPE_BOX = 0,
    PHY_COLLISION_SHAPE_CAPSULE,
    PHY_COLLISION_SHAPE_SPHERE,
    PHY_COLLISION_SHAPE_NONE,
};


struct collision_shape_handle_t
{
    unsigned type : 3;
    unsigned index : 28;
};

#define COLLISION_SHAPE_HANDLE(type, index)(struct collision_shape_handle_t){type, index}
#define INVALID_COLLISION_SHAPE_INDEX 0x1fffffff
#define INVALID_COLLISION_HANDLE COLLISION_SHAPE_HANDLE(PHY_COLLISION_SHAPE_NONE, INVALID_COLLISION_SHAPE_INDEX)



struct base_shape_t
{
    int type;
};

struct box_shape_t
{
    struct base_shape_t base;
    vec3_t size;
    vec3_t diag;
};

struct capsule_shape_t
{
    struct base_shape_t base;
    float radius;
    float height;
};

struct sphere_shape_t
{
    struct base_shape_t base;
    float radius;
};

struct rigid_body_collider_t
{
    struct base_collider_t base;
    struct collision_shape_handle_t collision_shape;

    mat3_t orientation;
    vec3_t angular_velocity;
};

#endif // PHY_COMMON_H
