#ifndef PHY_COMMON_H
#define PHY_COMMON_H

#include "../gmath/vector.h"
#include "../gmath/matrix.h"
#include "../containers/list.h"
//#include "pla_common.h"

/*
============================================
============================================
============================================
*/

struct collider_handle_t
{
    unsigned type : 3;
    unsigned index : 29;
};

struct dbvh_node_t
{
    int parent;
    int children[2];

    struct collider_handle_t collider;

    vec3_t max;
    vec3_t min;

    vec3_t expanded_max;
    vec3_t expanded_min;
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
    vec3_t position_on_plane;
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




#define PHY_INVALID_COLLIDER_INDEX  0x1fffffff
#define PHY_COLLIDER_HANDLE(type, index)(struct collider_handle_t){type, index}
#define PHY_INVALID_COLLIDER_HANDLE PHY_COLLIDER_HANDLE(PHY_COLLIDER_TYPE_NONE, PHY_INVALID_COLLIDER_INDEX)


struct collision_shape_handle_t
{
    unsigned type : 3;
    unsigned index : 29;
};

#define PHY_COLLISION_SHAPE_HANDLE(type, index)(struct collision_shape_handle_t){type, index}
#define PHY_INVALID_COLLISION_SHAPE_INDEX 0x1fffffff
#define PHY_INVALID_COLLISION_SHAPE_HANDLE PHY_COLLISION_SHAPE_HANDLE(PHY_COLLISION_SHAPE_NONE, PHY_INVALID_COLLISION_SHAPE_INDEX)



#define MIN_CONTACT_PENETRATION 0.0001



struct base_collider_t
{
    int type;
    int dbvh_node;
    vec3_t position;
//    vec3_t linear_velocity;
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
    vec3_t linear_velocity;
    vec3_t warp_position;
    unsigned int entity_index;
//    struct player_handle_t player_handle;
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

enum PHY_PROJECTILE_COLLIDER_FLAGS
{
    PHY_PROJECTILE_COLLIDER_FLAG_IGNORE_GRAVITY = 1,
    PHY_PROJECTILE_COLLIDER_HAS_CONTACTS = 1 << 1,
    PHY_PROJECTILE_COLLIDER_FLAG_FROZEN = 1 << 2,
};

struct projectile_collider_t
{
    struct base_collider_t base;
    vec3_t linear_velocity;
    float overbounce;
    float radius;
    int flags;
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
