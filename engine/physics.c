#include "physics.h"
//#include "phy_col.h"
#include "../common/containers/stack_list.h"
#include "../common/gmath/geometry.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "r_common.h"
#include "r_view.h"
#include "r_portal.h"
#include "r_dbg.h"
#include "player.h"

#include <stdio.h>

#define GRAVITY (0.98 * 0.016 * 0.5)
//#define GRAVITY 0.0
#define MAX_HORIZONTAL_VELOCITY 0.15
#define GROUND_FRICTION 0.9



struct stack_list_t phy_colliders[PHY_COLLIDER_TYPE_NONE];
struct stack_list_t phy_collision_shapes[PHY_COLLISION_SHAPE_NONE];

//struct bvh_node_t *phy_collision_bvh = NULL;
//int phy_collision_triangle_count = 0;
//struct collision_triangle_t *phy_collision_triangles = NULL;


struct stack_list_t phy_dbvh_nodes;
int phy_dbvh_root = -1;
struct list_t phy_collision_pairs;

void (*phy_collision_matrix[PHY_COLLIDER_TYPE_NONE][PHY_COLLIDER_TYPE_NONE])(struct collider_handle_t, struct collider_handle_t);


#ifdef __cplusplus
extern "C"
{
#endif


int32_t phy_Init()
{
    phy_dbvh_nodes.init(sizeof(struct dbvh_node_t), 512);
    phy_collision_pairs.init(sizeof(struct collision_pair_t), 512);
    phy_colliders[PHY_COLLIDER_TYPE_PLAYER].init(sizeof(struct player_collider_t), 512);
    phy_colliders[PHY_COLLIDER_TYPE_STATIC].init(sizeof(struct static_collider_t), 512);
    phy_colliders[PHY_COLLIDER_TYPE_PORTAL].init(sizeof(struct portal_collider_t), 512);
    phy_colliders[PHY_COLLIDER_TYPE_RIGID].init(sizeof(struct rigid_body_collider_t), 512);
    phy_colliders[PHY_COLLIDER_TYPE_PROJECTILE].init(sizeof(struct projectile_collider_t), 512);


    phy_collision_shapes[PHY_COLLISION_SHAPE_BOX].init(sizeof(struct box_shape_t), 512);
    phy_collision_shapes[PHY_COLLISION_SHAPE_CAPSULE].init(sizeof(struct capsule_shape_t), 512);
    phy_collision_shapes[PHY_COLLISION_SHAPE_SPHERE].init(sizeof(struct sphere_shape_t), 512);

    phy_collision_matrix[PHY_COLLIDER_TYPE_PLAYER][PHY_COLLIDER_TYPE_PORTAL] = phy_CollidePlayerPortal;
    phy_collision_matrix[PHY_COLLIDER_TYPE_PORTAL][PHY_COLLIDER_TYPE_PLAYER] = phy_CollidePlayerPortal;

    phy_collision_matrix[PHY_COLLIDER_TYPE_STATIC][PHY_COLLIDER_TYPE_PLAYER] = phy_CollidePlayerStatic;
    phy_collision_matrix[PHY_COLLIDER_TYPE_PLAYER][PHY_COLLIDER_TYPE_STATIC] = phy_CollidePlayerStatic;

    phy_collision_matrix[PHY_COLLIDER_TYPE_RIGID][PHY_COLLIDER_TYPE_STATIC] = phy_ColliderRigidStatic;
    phy_collision_matrix[PHY_COLLIDER_TYPE_STATIC][PHY_COLLIDER_TYPE_RIGID] = phy_ColliderRigidStatic;

    phy_collision_matrix[PHY_COLLIDER_TYPE_PROJECTILE][PHY_COLLIDER_TYPE_STATIC] = phy_CollideProjectileStatic;
    phy_collision_matrix[PHY_COLLIDER_TYPE_STATIC][PHY_COLLIDER_TYPE_PROJECTILE] = phy_CollideProjectileStatic;

    return 0;
}

void phy_Shutdown()
{

}


struct collider_handle_t phy_CreateCollider(int type)
{
    struct collider_handle_t handle = PHY_INVALID_COLLIDER_HANDLE;
    struct base_collider_t *collider;

    if(type >= PHY_COLLIDER_TYPE_PLAYER && type < PHY_COLLIDER_TYPE_NONE)
    {
//        printf("phy_CreateCollider\n");
        handle.type = type;
        handle.index = phy_colliders[type].add(NULL);

        collider = (struct base_collider_t *)phy_colliders[type].get(handle.index);
        collider->type = type;
        collider->position = vec3_t(0.0, 0.0, 0.0);
//        collider->linear_velocity = vec3_t(0.0, 0.0, 0.0);
        collider->dbvh_node = -1;
    }

    return handle;
}

struct collider_handle_t phy_CreateStaticCollider()
{
    struct collider_handle_t handle;
    struct static_collider_t *collider;

    handle = phy_CreateCollider(PHY_COLLIDER_TYPE_STATIC);
    collider = phy_GetStaticColliderPointer(handle);

    collider->vertices.init(sizeof(vec3_t), 24);

    return handle;
}

struct collider_handle_t phy_CreateProjectileCollider(const vec3_t &position, float radius)
{
    struct collider_handle_t handle;
    struct projectile_collider_t *collider;

    handle = phy_CreateCollider(PHY_COLLIDER_TYPE_PROJECTILE);
    collider = phy_GetProjectileColliderPointer(handle);

    collider->radius = radius;
    collider->overbounce = 1.0;
    collider->base.position = position;
    collider->linear_velocity = vec3_t(0.0, 0.0, 0.0);
    collider->flags = 0;

    return handle;
}

void phy_DestroyCollider(struct collider_handle_t collider_handle)
{
    struct base_collider_t *collider;

    collider = phy_GetColliderPointer(collider_handle);

    if(collider)
    {
        phy_RemoveNodeFromDbvh(collider->dbvh_node);
        phy_DeallocDbvhNode(collider->dbvh_node);
        collider->type = PHY_COLLIDER_TYPE_NONE;
        phy_colliders[collider_handle.type].remove(collider_handle.index);
    }
}

struct base_collider_t *phy_GetColliderPointer(struct collider_handle_t collider)
{
    struct base_collider_t *collider_ptr = NULL;

    if(collider.type >= PHY_COLLIDER_TYPE_PLAYER && collider.type < PHY_COLLIDER_TYPE_NONE)
    {
        if(collider.index != PHY_INVALID_COLLIDER_INDEX)
        {
            collider_ptr = (struct base_collider_t *)phy_colliders[collider.type].get(collider.index);

            if(collider_ptr->type == PHY_COLLIDER_TYPE_NONE)
            {
                collider_ptr = NULL;
            }
        }
    }

    return collider_ptr;
}


struct player_collider_t *phy_GetPlayerColliderPointer(struct collider_handle_t collider)
{
    struct player_collider_t *collider_ptr = NULL;

    if(collider.type == PHY_COLLIDER_TYPE_PLAYER)
    {
        collider_ptr = (struct player_collider_t *)phy_GetColliderPointer(collider);
    }

    return collider_ptr;
}

struct portal_collider_t *phy_GetPortalColliderPointer(struct collider_handle_t collider)
{
    struct portal_collider_t *collider_ptr = NULL;

    if(collider.type == PHY_COLLIDER_TYPE_PORTAL)
    {
        collider_ptr = (struct portal_collider_t *)phy_GetColliderPointer(collider);
    }

    return collider_ptr;
}

struct static_collider_t *phy_GetStaticColliderPointer(struct collider_handle_t collider)
{
    struct static_collider_t *collider_ptr = NULL;

    if(collider.type == PHY_COLLIDER_TYPE_STATIC)
    {
        collider_ptr = (struct static_collider_t *)phy_GetColliderPointer(collider);
    }

    return collider_ptr;
}

struct projectile_collider_t *phy_GetProjectileColliderPointer(struct collider_handle_t collider)
{
    struct projectile_collider_t *projectile_ptr = NULL;

    if(collider.type == PHY_COLLIDER_TYPE_PROJECTILE)
    {
        projectile_ptr = (struct projectile_collider_t *)phy_GetColliderPointer(collider);
    }

    return projectile_ptr;
}

struct rigid_body_collider_t *phy_GetRigidBodyColliderPointer(struct collider_handle_t collider)
{
    struct rigid_body_collider_t *collider_ptr = NULL;

    if(collider.type == PHY_COLLIDER_TYPE_RIGID)
    {
        collider_ptr = (struct rigid_body_collider_t *)phy_GetColliderPointer(collider);
    }

    return collider_ptr;
}



struct collision_shape_handle_t phy_AllocCollisionShape(int shape)
{
    struct base_shape_t *shape_ptr;
    struct collision_shape_handle_t handle = PHY_INVALID_COLLISION_SHAPE_HANDLE;

    if(shape < PHY_COLLISION_SHAPE_NONE)
    {
        handle.type = shape;
        handle.index = phy_collision_shapes[shape].add(NULL);
        shape_ptr = (struct base_shape_t *)phy_collision_shapes[shape].get(handle.index);
        shape_ptr->type = shape;
    }

    return handle;
}

void phy_FreeCollisionShape(struct collision_shape_handle_t shape)
{
    struct base_shape_t *shape_ptr;


    shape_ptr = phy_GetCollisionShapePointer(shape);

    if(shape_ptr)
    {
        shape_ptr->type = PHY_COLLISION_SHAPE_NONE;
        phy_collision_shapes[shape.type].remove(shape.index);
    }
}

struct base_shape_t *phy_GetCollisionShapePointer(struct collision_shape_handle_t shape)
{
    struct base_shape_t *shape_ptr = NULL;

    if(shape.type < PHY_COLLISION_SHAPE_NONE)
    {
        shape_ptr = (struct base_shape_t *)phy_collision_shapes[shape.type].get(shape.index);

        if(shape_ptr->type != shape.type)
        {
            shape_ptr = NULL;
        }
    }

    return shape_ptr;
}




void phy_Step()
{
//    int type;
//    int i;
//    int c;
//    struct player_collider_t *player_colliders;
//    struct player_collider_t *player_collider;
//
//    struct portal_collider_t *portal_colliders;
//    struct portal_collider_t *portal_collider;
//
//    struct collision_pair_t *collision_pairs;

    unsigned int i;
    struct collision_pair_t *collision_pairs;


    //phy_GenerateCollisionPairs();

    struct base_collider_t *colliders;
    struct base_collider_t *collider;
    struct player_collider_t *player_collider;
    struct portal_collider_t *portal_collider;
    struct static_collider_t *static_collider;
    struct projectile_collider_t *projectile_collider;
    struct rigid_body_collider_t *rigid_collider;
    struct base_shape_t *base_shape;
    struct box_shape_t *box_shape;

    struct collision_pair_t *pairs0;
    struct collision_pair_t *pairs1;
    struct collision_pair_t *pair0;
    struct collision_pair_t *pair1;

    unsigned int type;
    unsigned int c;

    phy_collision_pairs.cursor = 0;

//    player_collider = (struct player_collider_t *)phy_colliders[PHY_COLLIDER_TYPE_PLAYER].buffer;
//    c = phy_colliders[PHY_COLLIDER_TYPE_PLAYER].cursor;

    for(type = PHY_COLLIDER_TYPE_PLAYER; type < PHY_COLLIDER_TYPE_NONE; type++)
    {
        c = phy_colliders[type].cursor;

        switch(type)
        {
            case PHY_COLLIDER_TYPE_PLAYER:
                for(i = 0; i < c; i++)
                {
                    player_collider = (struct player_collider_t* )phy_colliders[type].get(i);

                    if(player_collider->base.type != PHY_COLLIDER_TYPE_PLAYER)
                    {
                        continue;
                    }

                    player_collider->flags &= ~PHY_PLAYER_COLLIDER_FLAG_ON_GROUND;
                }
            break;

            case PHY_COLLIDER_TYPE_PROJECTILE:
                for(i = 0; i < c; i++)
                {
                    projectile_collider = (struct projectile_collider_t* )phy_colliders[type].get(i);

                    if(projectile_collider->base.type != PHY_COLLIDER_TYPE_PROJECTILE)
                    {
                        continue;
                    }

                    projectile_collider->flags &= ~PHY_PROJECTILE_COLLIDER_HAS_CONTACTS;
                }
            break;
        }
    }


//    rigid_collider = (struct rigid_body_collider_t *)phy_colliders[PHY_COLLIDER_TYPE_RIGID].buffer;
//    c = phy_colliders[PHY_COLLIDER_TYPE_RIGID].cursor;
//
//
//    for(i = 0; i < c; i++)
//    {
//        if(rigid_collider[i].base.type == PHY_COLLIDER_TYPE_NONE)
//        {
//            continue;
//        }
//
//        base_shape = phy_GetCollisionShapePointer(rigid_collider[i].collision_shape);
//
//        switch(rigid_collider[i].collision_shape.type)
//        {
//            case PHY_COLLISION_SHAPE_BOX:
//                box_shape = (struct box_shape_t *)base_shape;
//                box_shape->diag = vfabs((box_shape->size * 0.5) * rigid_collider[i].orientation);
//            break;
//        }
//
//        rigid_collider[i].base.position += rigid_collider[i].base.linear_velocity * 0.001;
//    }






    /* generate collision pairs for all but static colliders... */
    for(type = PHY_COLLIDER_TYPE_PLAYER; type < PHY_COLLIDER_TYPE_NONE; type++)
    {
        if(type == PHY_COLLIDER_TYPE_STATIC)
        {
            continue;
        }

//        colliders = (struct base_collider_t *)phy_colliders[type].buffer;
        c = phy_colliders[type].cursor;

        for(i = 0; i < c; i++)
        {
//            collider = colliders + i;
            collider = (struct base_collider_t* )phy_colliders[type].get(i);

            if(collider->type != type)
            {
                continue;
            }

            phy_GenerateColliderCollisionPairs(PHY_COLLIDER_HANDLE(type, i));
        }
    }

    /* remove duplicate pairs... */
    pairs0 = (struct collision_pair_t *)phy_collision_pairs.buffer;

    for(i = 0; i < phy_collision_pairs.cursor; i++)
    {
        pair0 = pairs0 + i;

        pairs1 = (struct collision_pair_t *)phy_collision_pairs.buffer;

        for(c = i + 1; c < phy_collision_pairs.cursor; c++)
        {
            pair1 = pairs1 + c;

            if(pair0->collider_a.index == pair1->collider_a.index &&
               pair0->collider_a.type == pair1->collider_a.type &&
               pair0->collider_b.index == pair1->collider_b.index &&
               pair0->collider_b.type == pair1->collider_b.type)
            {
                phy_collision_pairs.remove(c);
                break;
            }
        }
    }


    /* collide colliders... */
    collision_pairs = (struct collision_pair_t *)phy_collision_pairs.buffer;
    for(i = 0; i < phy_collision_pairs.cursor; i++)
    {
        phy_Collide(collision_pairs[i].collider_a, collision_pairs[i].collider_b);
    }

    for(type = PHY_COLLIDER_TYPE_PLAYER; type < PHY_COLLIDER_TYPE_NONE; type++)
    {
        c = phy_colliders[type].cursor;

        switch(type)
        {
            case PHY_COLLIDER_TYPE_PLAYER:
                for(i = 0; i < c; i++)
                {
                    player_collider = (struct player_collider_t* )phy_colliders[type].get(i);

                    if(player_collider->base.type != PHY_COLLIDER_TYPE_PLAYER)
                    {
                        continue;
                    }

                    if(!(player_collider->flags & PHY_PLAYER_COLLIDER_FLAG_FROZEN))
                    {
//                        player_collider->linear_velocity.y = 0.0;
                        /* to avoid a collider getting frozen in place when it forms no collision pair -
                        which happened because previously its position was being updated only inside the
                        collision handling function - its position also gets updated here. During collision
                        handling, its position will be modified only if the movement hits something, which
                        means its velocity will need to be clipped, and an up to date position is necessary
                        to find out the next collision, if any.

                        When a movement doesn't hit something, the collision handling routine exits because
                        no further collision will happen, which means we can add the linear velocity to the
                        position here.

                        If the collider doesn't form any collision pair, its unmodified velocity will
                        just be added to its position... */
                        player_collider->base.position += player_collider->linear_velocity;


                        if(player_collider->flags & PHY_PLAYER_COLLIDER_FLAG_ON_GROUND)
                        {
                            player_collider->linear_velocity.x *= GROUND_FRICTION;
                            player_collider->linear_velocity.z *= GROUND_FRICTION;
                        }

                        player_collider->linear_velocity.y -= GRAVITY;
                    }
                }
            break;

            case PHY_COLLIDER_TYPE_PROJECTILE:
                for(i = 0; i < c; i++)
                {
                    projectile_collider = (struct projectile_collider_t* )phy_colliders[type].get(i);

                    if(projectile_collider->base.type != PHY_COLLIDER_TYPE_PROJECTILE)
                    {
                        continue;
                    }

                    if(!(projectile_collider->flags & PHY_PROJECTILE_COLLIDER_FLAG_FROZEN))
                    {
                        projectile_collider->base.position += projectile_collider->linear_velocity;

                        if(projectile_collider->flags & PHY_PROJECTILE_COLLIDER_HAS_CONTACTS)
                        {
                            projectile_collider->linear_velocity *= 0.98;
                        }

                        if(!(projectile_collider->flags & PHY_PROJECTILE_COLLIDER_FLAG_IGNORE_GRAVITY))
                        {
                            projectile_collider->linear_velocity.y -= GRAVITY;
                        }
                    }
                }
            break;
        }
    }

//    player_collider = (struct player_collider_t *)phy_colliders[PHY_COLLIDER_TYPE_PLAYER].buffer;
//
//
//    for(i = 0; i < c; i++)
//    {
//        if(player_collider[i].base.type == PHY_COLLIDER_TYPE_NONE)
//        {
//            continue;
//        }
//    }
}

/*
============================================
============================================
============================================
*/


void phy_Jump(struct collider_handle_t collider)
{
    struct player_collider_t *collider_ptr;

    if(collider.type == PHY_COLLIDER_TYPE_PLAYER)
    {
        collider_ptr = (struct player_collider_t *)phy_GetColliderPointer(collider);

        if(collider_ptr && (collider_ptr->flags & PHY_PLAYER_COLLIDER_FLAG_ON_GROUND))
        {
            collider_ptr->linear_velocity.y += 0.18;
        }
    }
}


void phy_Move(struct collider_handle_t collider, vec3_t acceleration)
{
    struct player_collider_t *collider_ptr;
    float speed;

    collider_ptr = phy_GetPlayerColliderPointer(collider);

    if(collider_ptr)
    {
        acceleration.x += collider_ptr->linear_velocity.x;
        acceleration.y = 0.0;
        acceleration.z += collider_ptr->linear_velocity.z;

        speed = acceleration.length();

        if(speed > MAX_HORIZONTAL_VELOCITY)
        {
            acceleration = normalize(acceleration) * MAX_HORIZONTAL_VELOCITY;
        }

        collider_ptr->linear_velocity.x = acceleration.x;
        collider_ptr->linear_velocity.z = acceleration.z;
    }
}

/*
============================================
============================================
============================================
*/


int phy_AllocDbvhNode()
{
    struct dbvh_node_t *node;
    int node_index;
//    printf("phy_AllocDbvhNode\n");
    node_index = phy_dbvh_nodes.add(NULL);
    node = (struct dbvh_node_t *)phy_dbvh_nodes.get(node_index);

    node->parent = -1;
    node->children[0] = -1;
    node->children[1] = -1;
    node->collider = PHY_INVALID_COLLIDER_HANDLE;

    return node_index;
}

void phy_DeallocDbvhNode(int node_index)
{
    struct dbvh_node_t *node;
    node = phy_GetDbvhNodePointer(node_index);

    if(node)
    {
        node->parent = node_index;
        node->children[0] = node_index;
        node->children[1] = node_index;
        phy_dbvh_nodes.remove(node_index);
    }
}

struct dbvh_node_t *phy_GetDbvhNodePointer(int node_index)
{
    struct dbvh_node_t *node = (struct dbvh_node_t *)phy_dbvh_nodes.get(node_index);

    if(node)
    {
        if(node->children[0] == node_index && node->children[1] == node_index && node->parent == node_index)
        {
            node = NULL;
        }
    }

    return node;
}

struct dbvh_node_t *phy_GetSiblingDbvhNodePointer(int node_index)
{
    struct dbvh_node_t *node;
    struct dbvh_node_t *parent;

    node = phy_GetDbvhNodePointer(node_index);
    parent = phy_GetDbvhNodePointer(node->parent);

    return phy_GetDbvhNodePointer(parent->children[parent->children[0] != node_index]);
}

void phy_GenerateColliderCollisionPairsRecursive(int cur_node_index, int node_index, float *smallest_area, int *smallest_index)
{
    struct dbvh_node_t *cur_node;
    struct dbvh_node_t *new_node;
    struct dbvh_node_t *node;
    struct dbvh_node_t *sibling;
    struct dbvh_node_t *parent;
    struct dbvh_node_t *parent_parent;
    struct collision_pair_t pair;
    float area;
    int i;
    int new_node_index;
    int parent_index;
    int sibling_index;

    vec3_t max;
    vec3_t min;

    vec3_t extents;

    if(phy_dbvh_root == -1)
    {
        phy_dbvh_root = node_index;
        return;
    }

    if(cur_node_index == node_index)
    {
        /* no reason to test collision between
        a node and itself... */
        return;
    }

    cur_node = phy_GetDbvhNodePointer(cur_node_index);
    node = phy_GetDbvhNodePointer(node_index);

    if(node->expanded_max.x >= cur_node->min.x && node->expanded_min.x <= cur_node->max.x)
    {
        if(node->expanded_max.y >= cur_node->min.y && node->expanded_min.y <= cur_node->max.y)
        {
            if(node->expanded_max.z >= cur_node->min.z && node->expanded_min.z <= cur_node->max.z)
            {
                if(cur_node->collider.index != PHY_INVALID_COLLIDER_INDEX)
                {
                    /* leaf node... */
                    if(node->collider.type < cur_node->collider.type)
                    {
                        pair.collider_a = node->collider;
                        pair.collider_b = cur_node->collider;
                    }
                    else
                    {
                        pair.collider_b = node->collider;
                        pair.collider_a = cur_node->collider;
                    }

                    phy_collision_pairs.add(&pair);
                }
                else
                {
                    phy_GenerateColliderCollisionPairsRecursive(cur_node->children[0], node_index, smallest_area, smallest_index);
                    phy_GenerateColliderCollisionPairsRecursive(cur_node->children[1], node_index, smallest_area, smallest_index);
                }
            }
        }
    }

    phy_DbvhNodesVolume(cur_node_index, node_index, &max, &min);

//    volume = (max.x - min.x) * (max.y - min.y) * (max.z - min.z);

    extents = max - min;

    /* smallest area heuristic... */
    area = ((extents.x * extents.z) +
            (extents.y * extents.z) +
            (extents.x * extents.y)) * 2.0;

    /* for every node touched during the hierarchy traversal, compute the volume
    formed by it and the node 'node'. If the area of the volume is the smallest,
    adjust the hierarchy... */
    if(area < *smallest_area)
    {
        /* if the expanded_extents of a node turn to be infinite,
        this code won't get executed, smallest_index will be -1,
        cur_node will be NULL and the whole thing will crash and
        burn... */
        *smallest_area = area;
        *smallest_index = cur_node_index;
    }

    if(cur_node_index == phy_dbvh_root)
    {
        cur_node_index = *smallest_index;

        /* 'cur_node' is now the node which node 'node' forms
        the smallest volume with... */
        cur_node = phy_GetDbvhNodePointer(cur_node_index);

        if(node->parent == -1)
        {
            /* node 'node' isn't in the hierarchy, so it'll get
            added here... */
            new_node_index = phy_AllocDbvhNode();
            new_node = phy_GetDbvhNodePointer(new_node_index);

            /* and here is where the problem of using linear buffers for growable arrays shines:
            whenever a resize happens, every pointer referencing the old buffer will be invalidated... */
            node = phy_GetDbvhNodePointer(node_index);
            cur_node = phy_GetDbvhNodePointer(cur_node_index);

            new_node->children[0] = node_index;
            new_node->children[1] = cur_node_index;

            if(cur_node->parent != -1)
            {
                /* 'cur_node' has a parent node, and it will be paired
                with node 'node'. So, make the parent node point to the
                newly created node, which will group 'cur_node' and 'node'
                together... */
                parent = phy_GetDbvhNodePointer(cur_node->parent);

                /* find which child of parent node 'cur_node' is and make it
                point to 'new_node'... */
                parent->children[parent->children[0] != cur_node_index] = new_node_index;

                /* 'new_node' now points to 'cur_node's parent... */
                new_node->parent = cur_node->parent;
            }
            else
            {
                /* 'cur_node' is the root of the hierarchy. This means 'new_node'
                will become the new root... */
                phy_dbvh_root = new_node_index;
            }

            /* make 'node' and 'cur_node' point to the 'new_node', which will
            group them into a new volume... */
            node->parent = new_node_index;
            cur_node->parent = new_node_index;

//            printf("new node added!\n");
        }
        else
        {
            /* node 'node' is already present in the hierarchy...  */

            if(node->parent == cur_node->parent)
            {
                /* 'cur_node' and 'node' are already in the correct configuration
                to form the smallest volume... */
                return;
            }
            else
            {
//                if(cur_node->children[0] == node_index ||
//                   cur_node->children[1] == node_index)
//                {
//                    /* if we get here, 'cur_node' will be the root, because 'node' is far away
//                    from the whole hierarchy and don't intersect it, which will make the root
//                    be the node with which 'node' forms the smallest volume possible. Whenever a
//                    node gets parented with the root, a new node gets created and becomes the new
//                    root. This only makes sense when 'node' isn't in the hierarchy... */
//                    return;
//                }

                if(node->parent == cur_node_index)
                {
                    /* don't parent 'node' with it's parent. It doesn't make
                    any sense... */
                    return;
                }


                /* again, 'cur_node' and 'node' form the smallest volume possible, but
                here, 'node' already has a sibling node. To avoid unecessarily alloc'ing
                and dealloc'ing nodes, the following happens:

                The sibling's parent node's parent node (yes, the parent node of its parent
                node), will point directly to the sibling node, and will forget about
                the sibling node's parent node. The sibling will then update its parent
                pointer to its parent node's parent node.

                #############################################################################

                                                (before)

                                             (parent's parent)
                                                _____|______
                                               |            |
                                            (parent)    (parent's sibling)
                                            ___|___
                                           |       |
                                        (node)  (sibling)

                -------------------------------------------------------------------------------

                                                (after)

                                            (parent's parent)
                                              ______|________
                                             |               |
                                         (sibling)      (parent's sibling)




                        <still points at parent's parent>
                                       |
                                    (parent)
                                    ___|___
                                   |       |
                                 (node)  <still points at sibling>

                #############################################################################

                After that, 'cur_node' will become 'node's new sibling'. 'cur_node' still points
                at its old parent node. 'cur_node's old parent becomes 'parent' new parent node.

                #############################################################################

                                                (before)

                                            (cur_node's parent)
                                              ______|________
                                             |               |
                                (cur_node's sibling)      (cur_node)

                -----------------------------------------------------------------------------

                                                (after)

                                            (cur_node's parent)
                                              ______|________
                                             |               |
                                (cur_node's sibling)        (parent)
                                                            ___|___
                                                           |       |
                                                         (node)  (cur_node)

                #############################################################################

                */

                parent = phy_GetDbvhNodePointer(node->parent);
                sibling_index = parent->children[0] == node_index;
                sibling = phy_GetDbvhNodePointer(parent->children[sibling_index]);
                sibling->parent = parent->parent;

                if(parent->parent != -1)
                {
                    /* 'parent' is not the root... */
                    parent_parent = phy_GetDbvhNodePointer(parent->parent);

                    /* 'node' sibling node becomes a direct child of 'parent's parent node... */
                    parent_parent->children[parent_parent->children[0] != node->parent] = parent->children[sibling_index];
                }
                else
                {
                    /* 'parent' is the root node. Since 'parent' and 'node will be moved
                    together, and 'sibling' would become a direct child of 'parent's parent
                    node, 'sibling' becomes the root instead, since 'parent' has no parent
                    node... */
                    phy_dbvh_root = parent->children[sibling_index];
                }




                if(cur_node_index == phy_dbvh_root)
                {
                    new_node_index = phy_AllocDbvhNode();
                    new_node = phy_GetDbvhNodePointer(new_node_index);

                    new_node->children[0] = node_index;
                    new_node->children[1] = cur_node_index;

                    node = phy_GetDbvhNodePointer(node_index);
                    cur_node = phy_GetDbvhNodePointer(cur_node_index);

                    cur_node->parent = new_node_index;
                    node->parent = new_node_index;

                    phy_dbvh_root = new_node_index;
                }
                else
                {
                    /* 'cur_node' becomes 'node's new sibling... */
                    parent->children[sibling_index] = cur_node_index;

                    /* 'cur_node's parent node now points to 'node's parent
                    node... */
                    parent_parent = phy_GetDbvhNodePointer(cur_node->parent);
                    parent_parent->children[parent_parent->children[0] != cur_node_index] = node->parent;

                    /* 'node's parent node now points to 'cur_node's old parent node... */
                    parent->parent = cur_node->parent;
                    /* 'cur_node' now points to 'node's parent node... */
                    cur_node->parent = node->parent;
                }
            }
        }
    }
}

void phy_RecomputeVolumesRecursive(int node_index)
{
    struct dbvh_node_t *node;

    node = phy_GetDbvhNodePointer(node_index);

    if(node)
    {
        if(node->collider.index == PHY_INVALID_COLLIDER_INDEX)
        {
            phy_DbvhNodesVolume(node->children[0], node->children[1], &node->max, &node->min);
        }

        phy_RecomputeVolumesRecursive(node->parent);
    }
}

void phy_GenerateColliderCollisionPairs(struct collider_handle_t collider)
{
    struct dbvh_node_t *node;
    struct base_collider_t *collider_ptr;
    struct player_collider_t *player_collider;
    struct projectile_collider_t *projectile_collider;
    struct portal_collider_t *portal_collider;
    struct static_collider_t *static_collider;
    struct rigid_body_collider_t *rigid_collider;
    struct base_shape_t *base_shape;
    struct box_shape_t *box_shape;
    struct portal_t *portal;

    float smallest_volume = FLT_MAX;
    int smallest_index = -1;

    vec2_t portal_size;
    vec3_t corners[4];
    vec3_t linear_velocity;

    vec3_t corner;
    vec3_t corner2;
    vec3_t size;

    int i;
    int j;

    collider_ptr = phy_GetColliderPointer(collider);

    if(collider_ptr)
    {
        if(collider_ptr->dbvh_node == -1)
        {
//            printf("phy_GenerateColliderCollisionPairs\n");
            /* this collider hasn't been inserted into the dbvh,
            so alloc a new node for it here... */
            collider_ptr->dbvh_node = phy_AllocDbvhNode();
        }

        node = phy_GetDbvhNodePointer(collider_ptr->dbvh_node);
        node->collider = collider;

        switch(collider_ptr->type)
        {
            case PHY_COLLIDER_TYPE_PLAYER:
                player_collider = (struct player_collider_t *)collider_ptr;

                linear_velocity = player_collider->linear_velocity;

                node->max = player_collider->base.position +
                            vec3_t(player_collider->radius,
                                   player_collider->height * 0.5,
                                   player_collider->radius);

                node->min = player_collider->base.position +
                            vec3_t(-player_collider->radius,
                                   -player_collider->height * 0.5,
                                   -player_collider->radius);

                node->expanded_max = node->max;
                node->expanded_min = node->min;

                for(i = 0; i < 3; i++)
                {
                    if(linear_velocity[i] > 0.0)
                    {
                        node->expanded_max[i] += linear_velocity[i];
                    }
                    else
                    {
                        node->expanded_min[i] += linear_velocity[i];
                    }
                }

                if(node->expanded_max[1] != node->expanded_max[1])
                {

                    printf("nan\n");
                }

            break;

            case PHY_COLLIDER_TYPE_PROJECTILE:
                projectile_collider = (struct projectile_collider_t *)collider_ptr;

                linear_velocity = projectile_collider->linear_velocity;

                node->max = projectile_collider->base.position +
                            vec3_t(projectile_collider->radius,
                                   projectile_collider->radius,
                                   projectile_collider->radius);

                node->min = projectile_collider->base.position +
                            vec3_t(-projectile_collider->radius,
                                   -projectile_collider->radius,
                                   -projectile_collider->radius);

                node->expanded_max = node->max;
                node->expanded_min = node->min;

                for(i = 0; i < 3; i++)
                {
                    if(linear_velocity[i] > 0.0)
                    {
                        node->expanded_max[i] += linear_velocity[i];
                    }
                    else
                    {
                        node->expanded_min[i] += linear_velocity[i];
                    }
                }
            break;

            case PHY_COLLIDER_TYPE_PORTAL:
                portal_collider = (struct portal_collider_t *)collider_ptr;
                portal = r_GetPortalPointer(portal_collider->portal_handle);

                node->max = vec3_t(-FLT_MAX, -FLT_MAX, -FLT_MAX);
                node->min = vec3_t(FLT_MAX, FLT_MAX, FLT_MAX);

                node->expanded_max = node->max;
                node->expanded_min = node->min;

                portal_size = portal->size * 0.5;

                corners[0] = vec3_t(portal_size[0], portal_size[1], 0.0);
                corners[1] = vec3_t(-portal_size[0], portal_size[1], 0.0);
                corners[2] = vec3_t(-portal_size[0], -portal_size[1], 0.0);
                corners[3] = vec3_t(portal_size[0], -portal_size[1], 0.0);

                for(i = 0; i < 4; i++)
                {
                    corners[i] = corners[i] * portal->orientation + portal->position;

                    for(j = 0; j < 3; j++)
                    {
                        if(corners[i][j] > node->max[j]) node->max[j] = corners[i][j];
                        if(corners[i][j] < node->min[j]) node->min[j] = corners[i][j];
                    }
                }

                for(i = 0; i < 3; i++)
                {
                    if(fabs(node->max[i] - node->min[i]) < 0.001)
                    {
                        node->max[i] += 0.05;
                        node->min[i] -= 0.05;
                    }
                }

            break;

            case PHY_COLLIDER_TYPE_STATIC:
                static_collider = (struct static_collider_t *)collider_ptr;
                node->max = static_collider->nodes->max;
                node->min = static_collider->nodes->min;

                node->expanded_max = node->max;
                node->expanded_min = node->min;
            break;

            case PHY_COLLIDER_TYPE_RIGID:
                rigid_collider = (struct rigid_body_collider_t *)collider_ptr;

                node->max = vec3_t(-FLT_MAX, -FLT_MAX, -FLT_MAX);
                node->min = vec3_t(FLT_MAX, FLT_MAX, FLT_MAX);

                node->expanded_max = node->max;
                node->expanded_min = node->min;

                base_shape = phy_GetCollisionShapePointer(rigid_collider->collision_shape);

                switch(rigid_collider->collision_shape.type)
                {
                    case PHY_COLLISION_SHAPE_BOX:
                        box_shape = (struct box_shape_t *)base_shape;

                        size = box_shape->size * 0.5;

                        corners[0] = vec3_t(-size[0], size[1], size[2]);
                        corners[1] = vec3_t(size[0], size[1], -size[2]);
                        corners[2] = vec3_t(size[0], -size[1], size[2]);
                        corners[3] = -size;

                        for(i = 0; i < 4; i++)
                        {
                            corner = corners[i] * rigid_collider->orientation + rigid_collider->base.position;

                            if(corner[0] > node->max[0]) node->max[0] = corner[0];
                            if(corner[1] > node->max[1]) node->max[1] = corner[1];
                            if(corner[2] > node->max[2]) node->max[2] = corner[2];

                            if(corner[0] < node->min[0]) node->min[0] = corner[0];
                            if(corner[1] < node->min[1]) node->min[1] = corner[1];
                            if(corner[2] < node->min[2]) node->min[2] = corner[2];
                        }

                    break;
                }

            break;
        }

//        phy_UpdateDbvhForNode(collider_ptr->dbvh_node);
        phy_GenerateColliderCollisionPairsRecursive(phy_dbvh_root, collider_ptr->dbvh_node, &smallest_volume, &smallest_index);
        phy_RecomputeVolumesRecursive(collider_ptr->dbvh_node);
    }
}

void phy_RemoveNodeFromDbvh(int node_index)
{
    struct dbvh_node_t *node;
    struct dbvh_node_t *sibling;
    struct dbvh_node_t *parent;
    struct dbvh_node_t *parent_parent;

    int sibling_index;

    node = phy_GetDbvhNodePointer(node_index);

    if(node)
    {
        if(node->parent == -1)
        {
            /* node is the current root... */


            /* setting the root to an invalid index is enough here, since
            only leaf nodes are accessible externally, and if a leaf node
            is also the root, it's the only node in the hierarchy... */
            phy_dbvh_root = -1;
        }
        else
        {
            /* 'node' is not the root, so we'll remove it and 'parent', and
            then set 'node's sibling node as direct child node of 'parent' parent node... */

            parent = phy_GetDbvhNodePointer(node->parent);
            sibling_index = parent->children[parent->children[0] == node_index];
            sibling = phy_GetDbvhNodePointer(sibling_index);

            if(parent->parent != -1)
            {
                /* 'parent' is not the root...

                ############################################################################

                                                  (before)

                                                      .
                                                      .
                                                      .
                                                      |
                                              (parent's parent)
                                              ________|________
                                             |                 |
                                          (parent)      (parent's sibling)
                                        _____|_____
                                       |           |
                                    (node)      (sibling)

                ------------------------------------------------------------------------------

                                                   (after)

                                                      .
                                                      .
                                                      .
                                                      |
                                              (parent's parent)
                                              ________|________
                                             |                 |
                                        (sibling)      (parent's sibling)

                ############################################################################

                */



                parent_parent = phy_GetDbvhNodePointer(parent->parent);
                parent_parent->children[parent_parent->children[0] != node->parent] = sibling_index;
                sibling->parent = parent->parent;
            }
            else
            {
                /* parent is the root...

                ############################################################################

                                                  (before)

                                            (parent - the root)
                                          ___________|___________
                                         |                       |
                                      (node)                 (sibling)
                                    _____|______            _____|_____
                                   |            |          |           |
                                   .            .          .           .
                                   .            .          .           .
                                   .            .          .           .


                ------------------------------------------------------------------------------

                                                   (after)

                                           (sibling - new root)
                                               _____|_____
                                              |           |
                                              .           .
                                              .           .
                                              .           .

                ############################################################################
                 */
                phy_dbvh_root = sibling_index;
            }

            phy_RecomputeVolumesRecursive(sibling_index);

            /* dealloc only 'parent', since 'node' is a leaf node, which is linked to
            a collider. Let external code decide whether it wants to get rid of 'node'... */
            phy_DeallocDbvhNode(node->parent);
            node->parent = -1;
        }
    }
}

void phy_GenerateCollisionPairs()
{
    struct base_collider_t *colliders;
    struct base_collider_t *collider;
    struct player_collider_t *player_collider;
    struct portal_collider_t *portal_collider;
    struct static_collider_t *static_collider;
    struct rigid_body_collider_t *rigid_collider;

    struct collision_pair_t *pairs0;
    struct collision_pair_t *pair0;
    struct collision_pair_t *pairs1;
    struct collision_pair_t *pair1;

    int type;
    unsigned int i;
    unsigned int c;

    phy_collision_pairs.cursor = 0;

    for(type = PHY_COLLIDER_TYPE_PLAYER; type < PHY_COLLIDER_TYPE_NONE; type++)
    {
        colliders = (struct base_collider_t *)phy_colliders[type].buffer;
        c = phy_colliders[type].cursor;

        for(i = 0; i < c; i++)
        {
//            collider = colliders + i;
            collider = (struct base_collider_t* )phy_colliders[type].get(i);

            if(collider->type != type)
            {
                continue;
            }

            phy_GenerateColliderCollisionPairs(PHY_COLLIDER_HANDLE(type, i));
        }
    }

    /* remove duplicate pairs... */
    pairs0 = (struct collision_pair_t *)phy_collision_pairs.buffer;

    for(i = 0; i < phy_collision_pairs.cursor; i++)
    {
        pair0 = pairs0 + i;

        pairs1 = (struct collision_pair_t *)phy_collision_pairs.buffer;

        for(c = i + 1; c < phy_collision_pairs.cursor; c++)
        {
            pair1 = pairs1 + c;

            if(pair0->collider_a.index == pair1->collider_a.index &&
               pair0->collider_a.type == pair1->collider_a.type &&
               pair0->collider_b.index == pair1->collider_b.index &&
               pair0->collider_b.type == pair1->collider_b.type)
            {
                phy_collision_pairs.remove(c);
                break;
            }
        }
    }
}

void phy_DbvhNodesVolume(int node_a, int node_b, vec3_t *max, vec3_t *min)
{
    struct dbvh_node_t *a_ptr;
    struct dbvh_node_t *b_ptr;

    int i;

    a_ptr = phy_GetDbvhNodePointer(node_a);
    b_ptr = phy_GetDbvhNodePointer(node_b);


    if(a_ptr && b_ptr)
    {
        *max = vec3_t(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        *min = vec3_t(FLT_MAX, FLT_MAX, FLT_MAX);

        for(i = 0; i < 3; i++)
        {
            if(a_ptr->max[i] > max->floats[i]) max->floats[i] = a_ptr->max[i];
            if(a_ptr->min[i] < min->floats[i]) min->floats[i] = a_ptr->min[i];
        }

        for(i = 0; i < 3; i++)
        {
            if(b_ptr->max[i] > max->floats[i]) max->floats[i] = b_ptr->max[i];
            if(b_ptr->min[i] < min->floats[i]) min->floats[i] = b_ptr->min[i];
        }
    }
}

/*
============================================
============================================
============================================
*/

void phy_ClearBvh(struct bvh_node_t *node)
{
    if(node)
    {
        phy_ClearBvh(node->left);
        phy_ClearBvh(node->right);
        free(node);
    }
}

struct bvh_node_t *phy_BuildBvhRecursive(struct bvh_node_t *nodes)
{
    struct bvh_node_t *new_nodes = NULL;
    struct bvh_node_t *new_node = NULL;
    struct bvh_node_t *prev_node;
    struct bvh_node_t *next_node;
    struct bvh_node_t *node = NULL;

    int i;

    vec3_t max;
    vec3_t min;
    vec3_t bounding_box;

    float volume;
    float min_volume;

    if(nodes->parent)
    {
        while(nodes)
        {
            new_node = (struct bvh_node_t *)calloc(sizeof(struct bvh_node_t), 1);
            new_node->left = nodes;
            new_node->data = NULL;
            nodes = nodes->parent;
            new_node->left->parent = new_node;

            new_node->max = new_node->left->max;
            new_node->min = new_node->left->min;

            if(nodes)
            {
                prev_node = NULL;
                node = nodes;
                min_volume = FLT_MAX;

                while(node)
                {
                    max = new_node->max;
                    min = new_node->min;

                    for(i = 0; i < 3; i++)
                    {
                        if(node->max[i] > max[i]) max[i] = node->max[i];
                        if(node->min[i] < min[i]) min[i] = node->min[i];
                    }

                    bounding_box = max - min;
                    volume = fabs(bounding_box.x * bounding_box.y * bounding_box.z);

                    if(volume < min_volume)
                    {
                        /* this node forms the smaller bounding volume so far... */
                        min_volume = volume;

                        next_node = node->parent;

                        if(prev_node)
                        {
                            prev_node->parent = node->parent;

                            /* make this node point to the start of the nodes list... */
                            node->parent = nodes;
                            /* make this node the beginning of the nodes list... */
                            nodes = node;
                        }

                        node = next_node;

                        continue;
                    }

                    prev_node = node;
                    node = node->parent;
                }

                /* the first element of the nodes list forms the smallest
                bounding volume, so we make it one of the children of new_node... */
                new_node->right = nodes;

                /* advance one node forward, given that nodes as is still points to
                the right child of the new_node... */
                nodes = nodes->parent;
                new_node->right->parent = new_node;

                node = new_node->right;

                for(i = 0; i < 3; i++)
                {
                    if(node->max[i] > new_node->max[i]) new_node->max[i] = node->max[i];
                    if(node->min[i] < new_node->min[i]) new_node->min[i] = node->min[i];
                }
            }
            else
            {
                /* this new_node doesn't have two children, which means it will
                actually make the search slower, given that it has the same volume
                as it's left child. To avoid the unnecessary indirection, we get
                rid of it here, and use its left child as the new node... */
                node = new_node->left;
                free(new_node);
                new_node = node;
            }

            /* use the parent pointer to link the new nodes, so they can be
            used by the next recursive call... */
            new_node->parent = new_nodes;
            new_nodes = new_node;
        }

        /* take the volumes generated here and group then into volumes... */
        return phy_BuildBvhRecursive(new_nodes);
    }
    else
    {
        return nodes;
    }
}


//void phy_BuildBvh(vec3_t *vertices, int vertice_count)
//{
//    int i;
//    int j;
//
//    struct bvh_node_t *nodes = NULL;
//    struct bvh_node_t *new_node = NULL;
//
//    vertice_count -= vertice_count % 3;
//
//    phy_collision_triangle_count = 0;
//
//    if(vertice_count)
//    {
//        phy_collision_triangles = (struct collision_triangle_t *)calloc(sizeof(collision_triangle_t), vertice_count / 3);
//
//        for(i = 0; i < vertice_count;)
//        {
//            phy_collision_triangles[phy_collision_triangle_count].normal = normalize(cross(vertices[i + 2] - vertices[i + 1], vertices[i + 1] - vertices[i]));
//
//            new_node = (struct bvh_node_t *)calloc(sizeof(struct bvh_node_t ), 1);
//            new_node->left = NULL;
//            new_node->right = NULL;
//
//            new_node->max = vec3_t(-FLT_MAX, -FLT_MAX, -FLT_MAX);
//            new_node->min = vec3_t(FLT_MAX, FLT_MAX, FLT_MAX);
//
//            for(j = 0; j < 3; j++, i++)
//            {
//                phy_collision_triangles[phy_collision_triangle_count].verts[j] = vertices[i];
//
//                if(vertices[i].x > new_node->max.x) new_node->max.x = vertices[i].x;
//                if(vertices[i].x < new_node->min.x) new_node->min.x = vertices[i].x;
//
//                if(vertices[i].y > new_node->max.y) new_node->max.y = vertices[i].y;
//                if(vertices[i].y < new_node->min.y) new_node->min.y = vertices[i].y;
//
//                if(vertices[i].z > new_node->max.z) new_node->max.z = vertices[i].z;
//                if(vertices[i].z < new_node->min.z) new_node->min.z = vertices[i].z;
//            }
//
//            for(j = 0; j < 3; j++)
//            {
//                if(fabs(new_node->max[j] - new_node->min[j]) < 0.001)
//                {
//                    new_node->max[j] += 0.05;
//                    new_node->min[j] -= 0.05;
//                }
//            }
//
//            new_node->data = phy_collision_triangles + phy_collision_triangle_count;
//
//
//            new_node->parent = nodes;
//            nodes = new_node;
//
//            phy_collision_triangle_count++;
//        }
//
//        phy_collision_bvh = phy_BuildBvhRecursive(nodes);
//    }
//}

void phy_AddStaticTriangles(struct collider_handle_t static_collider, vec3_t *vertices, int vertice_count)
{
    struct static_collider_t *collider;
    int i;

    collider = phy_GetStaticColliderPointer(static_collider);

    if(collider)
    {
        vertice_count -= vertice_count % 3;

        if(vertice_count)
        {
            collider->vertices.resize(vertice_count);

            for(i = 0; i < vertice_count; i++)
            {
                collider->vertices.add(vertices + i);
            }
        }
    }
}

void phy_StaticColliderBvh(struct collider_handle_t static_collider)
{
    struct static_collider_t *collider;
    struct bvh_node_t *new_node = NULL;
    struct bvh_node_t *nodes = NULL;
    vec3_t *vertices;
    unsigned int i;
    int j;

    collider = phy_GetStaticColliderPointer(static_collider);

    if(collider)
    {
        if(collider->vertices.cursor)
        {
            if(collider->collision_triangles)
            {
                free(collider->collision_triangles);
            }

            collider->collision_triangles_count = 0;
            collider->collision_triangles = (struct collision_triangle_t *)calloc(sizeof(collision_triangle_t), collider->vertices.cursor / 3);

            vertices = (vec3_t *)collider->vertices.buffer;

            for(i = 0; i < collider->vertices.cursor;)
            {
                collider->collision_triangles[collider->collision_triangles_count].normal = normalize(cross(vertices[i + 2] - vertices[i + 1], vertices[i + 1] - vertices[i]));

                new_node = (struct bvh_node_t *)calloc(sizeof(struct bvh_node_t ), 1);
                new_node->left = NULL;
                new_node->right = NULL;

                new_node->max = vec3_t(-FLT_MAX, -FLT_MAX, -FLT_MAX);
                new_node->min = vec3_t(FLT_MAX, FLT_MAX, FLT_MAX);

                for(j = 0; j < 3; j++, i++)
                {
                    collider->collision_triangles[collider->collision_triangles_count].verts[j] = vertices[i];

                    if(vertices[i].x > new_node->max.x) new_node->max.x = vertices[i].x;
                    if(vertices[i].x < new_node->min.x) new_node->min.x = vertices[i].x;

                    if(vertices[i].y > new_node->max.y) new_node->max.y = vertices[i].y;
                    if(vertices[i].y < new_node->min.y) new_node->min.y = vertices[i].y;

                    if(vertices[i].z > new_node->max.z) new_node->max.z = vertices[i].z;
                    if(vertices[i].z < new_node->min.z) new_node->min.z = vertices[i].z;
                }

                for(j = 0; j < 3; j++)
                {
                    if(fabs(new_node->max[j] - new_node->min[j]) < 0.1)
                    {
                        new_node->max[j] += 0.1;
                        new_node->min[j] -= 0.1;
                    }
                }

                new_node->data = collider->collision_triangles + collider->collision_triangles_count;

                new_node->parent = nodes;
                nodes = new_node;

                collider->collision_triangles_count++;
            }

            collider->nodes = phy_BuildBvhRecursive(nodes);

            phy_GenerateColliderCollisionPairs(static_collider);
        }
    }

}

void phy_BoxOnBvhRecursive(vec3_t &aabb_max, vec3_t &aabb_min, struct bvh_node_t *node, struct list_t *triangle_list)
{
    struct bvh_node_t *child;
    int i;

    if(node->data)
    {
        triangle_list->add(&node->data);
    }
    else
    {
        child = node->left;

        for(i = 0; i < 2; i++)
        {
//            r_DrawLine(child->max, vec3_t(child->max.x, child->min.y, child->max.z), vec3_t(1.0, 0.0, 0.0));
//            r_DrawLine(vec3_t(child->max.x, child->min.y, child->max.z), vec3_t(child->max.x, child->min.y, child->min.z), vec3_t(1.0, 0.0, 0.0));
//            r_DrawLine(vec3_t(child->max.x, child->min.y, child->min.z), vec3_t(child->max.x, child->max.y, child->min.z), vec3_t(1.0, 0.0, 0.0));
//            r_DrawLine(vec3_t(child->max.x, child->max.y, child->min.z), child->max, vec3_t(1.0, 0.0, 0.0));
//
//            r_DrawLine(vec3_t(child->min.x, child->max.y, child->max.z), vec3_t(child->min.x, child->min.y, child->max.z), vec3_t(1.0, 0.0, 0.0));
//            r_DrawLine(vec3_t(child->min.x, child->min.y, child->max.z), vec3_t(child->min.x, child->min.y, child->min.z), vec3_t(1.0, 0.0, 0.0));
//            r_DrawLine(vec3_t(child->min.x, child->min.y, child->min.z), vec3_t(child->min.x, child->max.y, child->min.z), vec3_t(1.0, 0.0, 0.0));
//            r_DrawLine(vec3_t(child->min.x, child->max.y, child->min.z), vec3_t(child->min.x, child->max.y, child->max.z), vec3_t(1.0, 0.0, 0.0));

            if(aabb_max.x >= child->min.x && aabb_min.x <= child->max.x)
            {
                if(aabb_max.y >= child->min.y && aabb_min.y <= child->max.y)
                {
                    if(aabb_max.z >= child->min.z && aabb_min.z <= child->max.z)
                    {
                        phy_BoxOnBvhRecursive(aabb_max, aabb_min, child, triangle_list);
                    }
                }
            }

            child = node->right;
        }
    }
}





#define MAX_COLLISION_TRIANGLES 2048
//struct collision_triangle_t *collision_triangles_buffer[MAX_COLLISION_TRIANGLES];
struct list_t collision_triangles(sizeof(struct collision_triangle_t **), MAX_COLLISION_TRIANGLES);

struct list_t *phy_BoxOnBvh(vec3_t &aabb_max, vec3_t &aabb_min, struct bvh_node_t *nodes)
{
    if(nodes)
    {
        collision_triangles.cursor = 0;
        phy_BoxOnBvhRecursive(aabb_max, aabb_min, nodes, &collision_triangles);

        return &collision_triangles;
    }

    return NULL;
}


#define MAX_CONTACT_POINTS 2048
//struct contact_point_t contact_points_buffer[MAX_CONTACT_POINTS];
//struct list_t contact_points(sizeof(struct contact_point_t), MAX_CONTACT_POINTS);

int phy_ClosestContactPlayerStatic(struct collider_handle_t player_collider, struct collider_handle_t static_collider, struct contact_point_t *contact_point)
{
    unsigned int i;
    int j;
    struct player_collider_t *collider;
    struct static_collider_t *static_col;
    struct list_t *triangles;
    struct collision_triangle_t *triangle;
//    struct collision_triangle_t *prev_triangle;
    struct capsule_shape_t capsule_shape;
    struct contact_point_t closest_contact;
    struct contact_point_t next_contact;

    vec3_t aabb_max;
    vec3_t aabb_min;
    vec3_t position;
    vec3_t extents;


    collider = phy_GetPlayerColliderPointer(player_collider);
    static_col = phy_GetStaticColliderPointer(static_collider);

    struct view_t *view = r_GetActiveView();

    if(collider && static_col)
    {
//        contact_points.cursor = 0;
        position = collider->base.position;

        extents = vec3_t(collider->radius, collider->height * 0.5, collider->radius);
        aabb_max = position + extents;
        aabb_min = position - extents;

        for(i = 0; i < 3; i++)
        {
            if(collider->linear_velocity[i] > 0.0)
            {
                aabb_max[i] += collider->linear_velocity[i];
            }
            else
            {
                aabb_min[i] += collider->linear_velocity[i];
            }
        }

        triangles = phy_BoxOnBvh(aabb_max, aabb_min, static_col->nodes);

        capsule_shape.height = collider->height;
        capsule_shape.radius = collider->radius;

//        printf("fuck\n");

        closest_contact.penetration = 1.0;
        closest_contact.position = collider->base.position + collider->linear_velocity;

        for(i = 0; i < triangles->cursor; i++)
        {
            triangle = *(struct collision_triangle_t **)triangles->get(i);

            int result = phy_ContactCapsuleTriangle2(triangle, &capsule_shape, collider->base.position, collider->linear_velocity, &next_contact);

            if(result)
            {
                if(result < 0)
                {
                    collider->flags |= PHY_PLAYER_COLLIDER_FLAG_FROZEN;
                }
                else if(next_contact.penetration < closest_contact.penetration)
                {
//                    contact_points.cursor = 0;
                    closest_contact = next_contact;
                }
            }

//            r_DrawTriangle(triangle->verts[0], triangle->verts[1], triangle->verts[2], vec3_t(0.0, 1.0, 0.0), 1, 0, 1);
        }

        *contact_point = closest_contact;
        return 1;
    }

    return 0;
}


int phy_ContactCapsuleTriangle(struct collision_triangle_t *triangle, struct capsule_shape_t *capsule_shape, vec3_t &position, vec3_t &velocity, struct contact_point_t *contact)
{
    vec3_t capsule_point;
    vec3_t movement_point;
    vec3_t triangle_point;
    vec3_t point;
    vec3_t normal;
    vec3_t dir;
    float col_t;
    float a;
    float b;
    int i;


    closest_point_line_triangle(triangle->verts[0], triangle->verts[1], triangle->verts[2],
                                    position + vec3_t(0.0, capsule_shape->height * 0.5 - capsule_shape->radius, 0.0),
                                    position - vec3_t(0.0, capsule_shape->height * 0.5 - capsule_shape->radius, 0.0),
                                    &triangle_point, &capsule_point, &col_t);

    dir = capsule_point - triangle_point;

//    if(dot(dir, dir) > capsule_shape->radius + 0.00001)
//    {
//        return 0;
//    }

    dir = normalize(dir);
    capsule_point = capsule_point + normalize(triangle_point - capsule_point) * capsule_shape->radius;
//    dir = normalize(capsule_point - triangle_point);

    b = dot(dir, (capsule_point + velocity) - triangle_point);

    if(b != 0.0)
    {
        a = dot(dir, capsule_point - triangle_point);
        col_t = a / (a - b);

        if(col_t >= 0.0 && col_t <= 1.0)
        {
            contact->penetration = col_t;
            contact->surface_normal = triangle->normal;
            contact->normal = dir;
            contact->position = position + velocity * col_t + dir * 0.0001;
            contact->position_on_plane = triangle_point;
            return 1;
        }
    }

    return 0;
}

int phy_ContactCapsuleTriangle2(struct collision_triangle_t *triangle, struct capsule_shape_t *capsule_shape, vec3_t &position, vec3_t &velocity, struct contact_point_t *contact)
{
    vec3_t triangle_capsule_vec;
    vec3_t triangle_point;
    vec3_t movement_point;
    vec3_t capsule_point;
    vec3_t point;
    vec3_t dir;
    vec3_t r;
    vec3_t normalized_velocity;
    float d;
    float velocity_len;
    float triangle_capsule_dist;
    float col_t;

    closest_point_line_triangle(triangle->verts[0], triangle->verts[1], triangle->verts[2],
                                    position + vec3_t(0.0, capsule_shape->height * 0.5 - capsule_shape->radius, 0.0),
                                    position - vec3_t(0.0, capsule_shape->height * 0.5 - capsule_shape->radius, 0.0),
                                    &triangle_point, &capsule_point, &col_t);

    closest_point_line_triangle(triangle->verts[0], triangle->verts[1], triangle->verts[2],
                                capsule_point, capsule_point + velocity, &point, &movement_point, &col_t);

    dir = point - movement_point;


    if(length(dir) > capsule_shape->radius)
    {
        return 0;
    }

    velocity_len = length(velocity);
    triangle_capsule_vec = triangle_point - capsule_point;
    triangle_capsule_dist = length(triangle_capsule_vec);
    normalized_velocity = velocity / velocity_len;
    triangle_capsule_vec /= triangle_capsule_dist;

    d = (-2.0 * triangle_capsule_dist * dot(normalized_velocity, triangle_capsule_vec));

    if(d != 0.0)
    {
        d = (capsule_shape->radius * capsule_shape->radius -
         triangle_capsule_dist * triangle_capsule_dist) / d;
    }

    if(velocity_len > 0.0)
    {
        col_t = d / velocity_len;

        if(col_t > 1.0)
        {
            col_t = 1.0;
        }
        else if(col_t < 0.0)
        {
            col_t = 0.0;
        }
    }
    else
    {
        col_t = 0.0;
    }

    point = capsule_point + velocity * col_t;
    triangle_point = closest_point_on_triangle(point, triangle->verts[0], triangle->verts[1], triangle->verts[2]);
    dir = normalize(point - triangle_point);

    contact->position = position + velocity * col_t + dir * 0.0001;
    contact->normal = dir;
    contact->surface_normal = triangle->normal;
    contact->penetration = col_t;

    return 1;
}


int phy_ClosestContactProjectileStatic(struct collider_handle_t projectile_collider, struct collider_handle_t static_collider, struct contact_point_t *contact_point)
{
    unsigned int i;
    int triangle_index = -1;
    int j;
    struct projectile_collider_t *collider;
    struct static_collider_t *static_col;
    struct list_t *triangles;
    struct collision_triangle_t *triangle;
    struct collision_triangle_t *prev_triangle;
    struct sphere_shape_t sphere_shape;
    struct contact_point_t closest_contact;
    struct contact_point_t next_contact;

    vec3_t aabb_max;
    vec3_t aabb_min;
    vec3_t position;
    vec3_t extents;


    collider = phy_GetProjectileColliderPointer(projectile_collider);
    static_col = phy_GetStaticColliderPointer(static_collider);

    struct view_t *view = r_GetActiveView();

    if(collider && static_col)
    {
//        contact_points.cursor = 0;
        position = collider->base.position;

        extents = vec3_t(collider->radius, collider->radius, collider->radius);
        aabb_max = position + extents;
        aabb_min = position - extents;

        for(i = 0; i < 3; i++)
        {
            if(collider->linear_velocity[i] > 0.0)
            {
                aabb_max[i] += collider->linear_velocity[i];
            }
            else
            {
                aabb_min[i] += collider->linear_velocity[i];
            }
        }

        triangles = phy_BoxOnBvh(aabb_max, aabb_min, static_col->nodes);

        sphere_shape.radius = collider->radius;

        closest_contact.penetration = 1.0;
        closest_contact.position = collider->base.position + collider->linear_velocity;

        for(i = 0; i < triangles->cursor; i++)
        {
            triangle = *(struct collision_triangle_t **)triangles->get(i);

            if(phy_ContactSphereTriangle(triangle, &sphere_shape, collider->base.position, collider->linear_velocity, &next_contact))
            {
                if(next_contact.penetration < closest_contact.penetration)
                {
                    closest_contact = next_contact;
                }
            }
        }

        *contact_point = closest_contact;

        return 1;
    }

    return 0;
}

int phy_ContactSphereTriangle(struct collision_triangle_t *triangle, struct sphere_shape_t *sphere_shape, vec3_t &position, vec3_t &velocity, struct contact_point_t *contact)
{
    vec3_t triangle_point;
    vec3_t movement_point;
    vec3_t point;
    vec3_t dir;
    float col_t;
    float movement_t;
    float a;
    float b;

    if(dot(velocity, triangle->normal) >= 0.0)
    {
        /* TODO: deal with this case... */
        return 0;
    }

    /* find the point in the movement that's 'sphere_shape->radius' away from the triangle's
    plane... */
    col_t = (sphere_shape->radius + dot(triangle->verts[0] - position, triangle->normal)) / dot(velocity, triangle->normal);
    movement_point = position + velocity * col_t;

    /* find the closest point on the triangle to 'movement_point' */
    triangle_point = closest_point_on_triangle(movement_point, triangle->verts[0], triangle->verts[1], triangle->verts[2]);

    /* move it alongside the triangle's normal, scaled by the sphere's radius,
    so we have a nice plane to raycast against... */
    point = triangle_point + triangle->normal * sphere_shape->radius;

    if(length(movement_point - point) > sphere_shape->radius + 0.00001)
    {
        return 0;
    }

    dir = normalize(movement_point - triangle_point);

    b = dot(dir, (position + velocity) - point);

    if(b != 0.0)
    {
        a = dot(dir, position - point);

        col_t = a / (a - b);

        if(col_t >= 0.0 && col_t <= 1.0)
        {
            contact->penetration = col_t;
            contact->position = position + velocity * col_t + dir * 0.0001;
            contact->surface_normal = triangle->normal;
            contact->normal = dir;
            return 1;
        }
    }

    return 0;
}

int phy_ClosestContactRigidStatic(struct collider_handle_t rigid_body_collider, struct collider_handle_t static_collider, struct contact_point_t *contact_point)
{
//    struct rigid_body_collider_t *rigid_body;
//    struct static_collider_t *static_col;
//    struct box_shape_t *box_shape;
//    struct dbvh_node_t *node;
//
//    struct contact_point_t contact;
//    contact_points.cursor = 0;
//
//    rigid_body = phy_GetRigidBodyColliderPointer(rigid_body_collider);
//    static_col = phy_GetStaticColliderPointer(static_collider);
//
//    vec3_t closest_on_plane;
//    vec3_t static_rigid_vec;
//    struct list_t *triangles;
//    struct collision_triangle_t *triangle;
//    unsigned int i;
//    float dist;
//    float radius;
//    vec3_t diag;
//
//    if(rigid_body && static_col)
//    {
//        box_shape = (struct box_shape_t *)phy_GetCollisionShapePointer(rigid_body->collision_shape);
//        node = phy_GetDbvhNodePointer(rigid_body->base.dbvh_node);
//
//        triangles = phy_BoxOnBvh(node->max, node->min, static_col->nodes);
//
//        for(i = 0; i < triangles->cursor; i++)
//        {
//            triangle = *(struct collision_triangle_t **)triangles->get(i);
//
//            if(phy_ContactBoxTriangle(triangle, box_shape, rigid_body->base.position, rigid_body->orientation, &contact))
//            {
//                contact_points.add(&contact);
//            }
//        }
//    }
}

int phy_ClosestContactPlayerPortal(struct collider_handle_t player_collider, struct collider_handle_t portal_collider, struct contact_point_t *contact_point)
{
//    struct player_collider_t *player_col;
//    struct portal_collider_t *portal_col;
//    struct portal_t *portal;
//
//    mat3_t inv_portal_orientation;
//    vec3_t portal_capsule_vec;
//    vec3_t closest_on_portal;
//    vec3_t closest_on_capsule;
//    vec2_t size;
//
//    struct contact_point_t contact;
//
//    vec3_t capsule_top;
//    vec3_t capsule_vec;
//
//    float t;
//    float dist;
//
//    contact_points.cursor = 0;
//
//    player_col = phy_GetPlayerColliderPointer(player_collider);
//    portal_col = phy_GetPortalColliderPointer(portal_collider);
//
//    if(player_col && portal_col)
//    {
//        portal = r_GetPortalPointer(portal_col->portal_handle);
//
//        portal_capsule_vec = player_col->base.position - portal_col->base.position;
//
//        inv_portal_orientation = portal->orientation;
//        inv_portal_orientation.transpose();
//
//        closest_on_portal = portal_capsule_vec * inv_portal_orientation;
//        size = portal->size * 0.5;
//
//        if(closest_on_portal.x > size.x) closest_on_portal.x = size.x;
//        else if(closest_on_portal.x < -size.x) closest_on_portal.x = -size.x;
//
//        if(closest_on_portal.y > size.y) closest_on_portal.y = size.y;
//        else if(closest_on_portal.y < -size.y) closest_on_portal.y = -size.y;
//
//        closest_on_portal.z = 0.0;
//
//        closest_on_portal = closest_on_portal * portal->orientation + portal->position;
//
//
//        capsule_top = player_col->base.position + vec3_t(0.0, player_col->height * 0.5, 0.0);
//        capsule_vec = vec3_t(0.0, -1.0, 0.0);
//
//        t = dot(closest_on_portal - capsule_top, capsule_vec);
//
//        closest_on_capsule = capsule_top + capsule_vec * t;
//
//        portal_capsule_vec = closest_on_capsule - closest_on_portal;
//
//        dist = length(portal_capsule_vec);
//
//        if(dist < player_col->radius + 0.05)
//        {
//            if(dist < 0.0001)
//            {
//                portal_capsule_vec = portal->orientation[2];
//            }
//            else
//            {
//                portal_capsule_vec /= dist;
//            }
//
//            contact.position = closest_on_portal;
//            contact.normal = portal_capsule_vec;
//            contact.penetration = player_col->radius - dist;
//            contact_points.add(&contact);
//        }
//
//        return &contact_points;
//    }
//
//    return NULL;
}




void phy_Collide(struct collider_handle_t collider_a, struct collider_handle_t collider_b)
{
    struct base_collider_t *ptr_a;
    struct base_collider_t *ptr_b;

    struct collider_handle_t temp;


    ptr_a = phy_GetColliderPointer(collider_a);
    ptr_b = phy_GetColliderPointer(collider_b);

    if(ptr_a && ptr_b)
    {
        if(collider_a.type > collider_b.type)
        {
            temp = collider_a;
            collider_a = collider_b;
            collider_b = temp;
        }

        if(phy_collision_matrix[collider_a.type][collider_b.type])
        {
            phy_collision_matrix[collider_a.type][collider_b.type](collider_a, collider_b);
        }
    }
}


void phy_SortContactPoints(struct contact_point_t *contacts, int left, int right)
{
//    struct contact_point_t *middle;
//    struct contact_point_t temp;
//    int i = left;
//    int j = right;
//
//    float angle;
//
//    middle = contacts + (left + right) / 2;
//
//    angle = dot(middle->normal, middle->surface_normal);
//
//    while(i <= j)
//    {
//        for(; i <= right && dot(contacts[i].normal, contacts[i].surface_normal) > angle; i++);
//        for(; j >= left && dot(contacts[j].normal, contacts[j].surface_normal) < angle; j--);
//
//        if(i >= j)
//        {
//            temp = contacts[i];
//            contacts[i] = contacts[j];
//            contacts[j] = temp;
//
//            i++;
//            j--;
//        }
//    }
//
//    if()
}



#define MAX_ITERATIONS 8
void phy_CollidePlayerStatic(struct collider_handle_t player_collider, struct collider_handle_t static_collider)
{
    struct list_t *contacts;
    unsigned int i;
    int j;
    int k;
//    int deepest_index;
    int closest_index;
    struct contact_point_t contact;
    struct player_collider_t *collider;
    struct static_collider_t *static_col;

    collider = phy_GetPlayerColliderPointer(player_collider);
    static_col = phy_GetStaticColliderPointer(static_collider);

    if(collider && static_col)
    {
        collider->flags &= ~PHY_PLAYER_COLLIDER_FLAG_ON_GROUND;

        for(j = 0; j < MAX_ITERATIONS; j++)
        {
            if(phy_ClosestContactPlayerStatic(player_collider, static_collider, &contact))
            {
                if(collider->linear_velocity.x == 0.0 &&
                   collider->linear_velocity.y == 0.0 &&
                   collider->linear_velocity.z == 0.0)
                {
                    break;
                }

                if(collider->flags & PHY_PLAYER_COLLIDER_FLAG_FROZEN)
                {
                    continue;
                }

//                contact = (struct contact_point_t *)contacts->get(0);

                if(contact.penetration == 1.0)
                {
                    break;
                }

                if(contact.penetration >= 0.0)
                {
                    collider->base.position = contact.position;
                }

//                if(dot(collider->base.linear_velocity, contact->surface_normal) <= 0.0)
                {
                    if(dot(contact.surface_normal, vec3_t(0.0, 1.0, 0.0)) > 0.7)
                    {
                        collider->flags |= PHY_PLAYER_COLLIDER_FLAG_ON_GROUND;
                    }

                    collider->linear_velocity = collider->linear_velocity - contact.normal *
                        dot(collider->linear_velocity, contact.normal);
                }
            }
            else
            {
                /* probably flying... */
                break;
            }
        }
    }
}

#define PHY_MAX_PROJECTILE_COLLISION_INTERACTIONS 128
void phy_CollideProjectileStatic(struct collider_handle_t projectile_collider, struct collider_handle_t static_collider)
{
    struct list_t *contacts;
    unsigned int i;
    int j;
    int k;
//    int deepest_index;
    int closest_index;
    struct contact_point_t contact;
    struct projectile_collider_t *collider;
    struct static_collider_t *static_col;

    collider = phy_GetProjectileColliderPointer(projectile_collider);
    static_col = phy_GetStaticColliderPointer(static_collider);

    if(collider && static_col)
    {
//        collider->flags &= ~PHY_PLAYER_COLLIDER_FLAG_ON_GROUND;

        for(j = 0; j < PHY_MAX_PROJECTILE_COLLISION_INTERACTIONS; j++)
        {
            if(phy_ClosestContactProjectileStatic(projectile_collider, static_collider, &contact))
            {
                if(collider->linear_velocity.x == 0.0 &&
                   collider->linear_velocity.y == 0.0 &&
                   collider->linear_velocity.z == 0.0)
                {
                    break;
                }

                if(collider->flags & PHY_PROJECTILE_COLLIDER_FLAG_FROZEN)
                {
                    continue;
                }

//                contact = (struct contact_point_t *)contacts->get(0);

                if(contact.penetration == 1.0)
                {
                    break;
                }

                if(contact.penetration > 0.0)
                {
                    collider->base.position = contact.position;
                }

                collider->linear_velocity = (collider->linear_velocity - contact.normal *
                    dot(collider->linear_velocity, contact.normal) * collider->overbounce);

                collider->flags |= PHY_PROJECTILE_COLLIDER_HAS_CONTACTS;
            }
            else
            {
                /* probably flying... */
                break;
            }
        }
    }
}

void phy_CollidePlayerPortal(struct collider_handle_t player_collider, struct collider_handle_t portal_collider)
{
//    struct player_collider_t *player;
//    struct portal_collider_t *portal;
//    struct portal_t *portal_ptr;
//    struct player_t *player_ptr;
//
//    struct list_t *contacts;
//    struct contact_point_t *contact;
//
//    vec3_t portal_capsule_vec;
//    vec3_t offset;
//
//    unsigned int i;
//
//    player = phy_GetPlayerColliderPointer(player_collider);
//    portal = phy_GetPortalColliderPointer(portal_collider);
//
//    if(portal && player)
//    {
//        contacts = phy_FindContactPointsPlayerPortal(player_collider, portal_collider);
//
//        if(contacts->cursor)
//        {
//            portal_ptr = r_GetPortalPointer(portal->portal_handle);
//            player_ptr = player_GetPlayerPointer(player->player_handle);
//
//            for(i = 0; i < contacts->cursor; i++)
//            {
//                contact = (struct contact_point_t *)contacts->get(i);
//
//                if(dot(player->base.position + player_ptr->camera_position - portal_ptr->position, portal_ptr->orientation[2]) > 0.0)
//                {
//                    portal_ptr = r_GetPortalPointer(portal_ptr->linked);
//
//                    portal_capsule_vec = player->base.position - portal->base.position;
//
//                    offset[0] = dot(portal_capsule_vec, portal_ptr->orientation[0]);
//                    offset[1] = dot(portal_capsule_vec, portal_ptr->orientation[1]);
//                    offset[2] = 0.0;
//
//                    player->base.position = portal_ptr->position +
//                                            offset[0] * portal_ptr->orientation[0] +
//                                            offset[1] * portal_ptr->orientation[1] -
//                                            portal_ptr->orientation[2] * 0.3;
//
//                    //printf("%f\n", dot(contact->normal, portal_ptr->orientation[2]));
//
//                    break;
//                }
//                //printf("[%f %f %f]\n", contact->normal[0], contact->normal[1], contact->normal[2]);
//            }
//        }
//    }
}

void phy_ColliderRigidStatic(struct collider_handle_t rigid_collider, struct collider_handle_t static_collider)
{
//    struct list_t *contacts;
//    int i;
//    int j;
//    int k;
//    int deepest_index;
//    struct contact_point_t *contact;
//    struct rigid_body_collider_t *collider;
//    struct static_collider_t *static_col;
//
//    vec3_t pos_correction;
//    vec3_t vel_correction;
//
//    /* hmm... */
//    float deepest;
//    float dist;
//    float greatest_angle = -FLT_MAX;
//    float angle;
//
//    collider = phy_GetRigidBodyColliderPointer(rigid_collider);
//    static_col = phy_GetStaticColliderPointer(static_collider);
//
//    if(collider && static_col)
//    {
//        contacts = phy_FindContactPointsRigidStatic(rigid_collider, static_collider);
//        for(j = 0; j < MAX_ITERATIONS; j++)
//        {
//            contacts = phy_FindContactPointsPlayerStatic(player_collider, static_collider);
//            pos_correction = vec3_t(0.0, 0.0, 0.0);
//
//            //printf("start\n");
//
//            if(contacts->cursor)
//            {
//                //while(contacts->cursor)
//                {
//                    deepest = -FLT_MAX;
//                    greatest_angle = 0.0;
//                    deepest_index = -1;
//
//                    for(i = 0; i < contacts->cursor; i++)
//                    {
//                        contact = (struct contact_point_t *)contacts->get(i);
//                        angle = dot(contact->normal, contact->surface_normal);
//
//                       /* printf("[%f %f %f] -- %f\n", contact->normal[0],
//                                               contact->normal[1],
//                                               contact->normal[2], contact->penetration);*/
//
////                        if(contact->penetration < 0.0001)
////                        {
////                            continue;
////                        }
//
//                        //if(angle > greatest_angle)
//                        if(contact->penetration > deepest)
//                        {
//                            deepest = contact->penetration;
//                            //greatest_angle = angle;
//                            deepest_index = i;
//                        }
//                    }
//
//                    if(deepest_index == -1)
//                    {
//                        break;
//                    }
//
//                    contact = (struct contact_point_t *)contacts->get(deepest_index);
//
//                    if(dot(contact->normal, vec3_t(0.0, 1.0, 0.0)) > 0.7)
//                    {
//                        collider->flags |= PHY_PLAYER_COLLIDER_FLAG_ON_GROUND;
//                    }
//
//                    if(dot(collider->base.linear_velocity, contact->normal) < 0.0)
//                    {
//                        /* clip the linear velocity to make it go parallel to the contact plane... */
//                        collider->base.linear_velocity = collider->base.linear_velocity - contact->normal *
//                                                    dot(collider->base.linear_velocity, contact->normal);
//                    }
//
//                    collider->base.position += contact->normal * contact->penetration;
//
//                    contacts->remove(deepest_index);
//                }
//            }
//            else
//            {
//                /* probably flying... */
//                break;
//            }
//        }
//    }
}




#ifdef __cplusplus
}
#endif








