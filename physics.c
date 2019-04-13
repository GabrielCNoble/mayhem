#include "physics.h"
#include "stack_list.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "r_common.h"
#include "r_view.h"
#include "r_portal.h"

#include <stdio.h>

#define GRAVITY (0.98 * 0.016 * 0.5)
#define MAX_HORIZONTAL_VELOCITY 0.15
#define GROUND_FRICTION 0.9



struct stack_list_t phy_colliders[PHY_COLLIDER_TYPE_NONE];

struct bvh_node_t *phy_collision_bvh = NULL;
int phy_collision_triangle_count = 0;
struct collision_triangle_t *phy_collision_triangles = NULL;


struct stack_list_t phy_dbvh_nodes;
int phy_dbvh_root = -1;
struct list_t phy_collision_pairs;


#ifdef __cplusplus
extern "C"
{
#endif


void phy_Init()
{
    phy_dbvh_nodes.init(sizeof(struct dbvh_node_t), 256);
    phy_collision_pairs.init(sizeof(struct collision_pair_t), 512);
    phy_colliders[PHY_COLLIDER_TYPE_PLAYER].init(sizeof(struct player_collider_t), 32);
    phy_colliders[PHY_COLLIDER_TYPE_PORTAL].init(sizeof(struct portal_collider_t), 32);
}

void phy_Shutdown()
{

}


struct collider_handle_t phy_CreateCollider(int type)
{
    struct collider_handle_t handle = INVALID_COLLIDER_HANDLE;
    struct base_collider_t *collider;

    if(type >= PHY_COLLIDER_TYPE_PLAYER && type < PHY_COLLIDER_TYPE_NONE)
    {
        handle.type = type;
        handle.index = phy_colliders[type].add(NULL);

        collider = (struct base_collider_t *)phy_colliders[type].get(handle.index);
        collider->type = type;
        collider->position = vec3_t(0.0, 0.0, 0.0);
        collider->linear_velocity = vec3_t(0.0, 0.0, 0.0);
        collider->dbvh_node = -1;
    }

    return handle;
}

void phy_DestroyCollider(struct collider_handle_t collider_handle)
{
    struct base_collider_t *collider;

    collider = phy_GetColliderPointer(collider_handle);

    if(collider)
    {
        collider->type = PHY_COLLIDER_TYPE_NONE;
        phy_colliders[collider_handle.type].remove(collider_handle.index);
    }
}

struct base_collider_t *phy_GetColliderPointer(struct collider_handle_t collider)
{
    struct base_collider_t *collider_ptr = NULL;

    if(collider.type >= PHY_COLLIDER_TYPE_PLAYER && collider.type < PHY_COLLIDER_TYPE_NONE)
    {
        if(collider.index != INVALID_COLLIDER_INDEX)
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


void phy_UpdateColliders()
{
    int type;
    int i;
    int c;
    struct player_collider_t *player_colliders;
    struct player_collider_t *player_collider;

    struct portal_collider_t *portal_colliders;
    struct portal_collider_t *portal_collider;

    struct collision_pair_t *collision_pairs;

    phy_collision_pairs.cursor = 0;

    for(type = PHY_COLLIDER_TYPE_PLAYER; type < PHY_COLLIDER_TYPE_NONE; type++)
    {
        switch(type)
        {
            case PHY_COLLIDER_TYPE_PLAYER:
                player_colliders = (struct player_collider_t *)phy_colliders[type].buffer;

                c = phy_colliders[type].cursor;

                for(i = 0; i < c; i++)
                {
                    player_collider = player_colliders + i;

                    if(player_collider->base.type != PHY_COLLIDER_TYPE_NONE)
                    {
                        player_collider->base.position += player_collider->base.linear_velocity;

                        phy_GenerateCollisionPairs(COLLIDER_HANDLE(PHY_COLLIDER_TYPE_PLAYER, i));

                        phy_CollidePlayerWorld(COLLIDER_HANDLE(PHY_COLLIDER_TYPE_PLAYER, i));

                        if(player_collider->flags & PHY_PLAYER_COLLIDER_FLAG_ON_GROUND)
                        {
                            player_collider->base.linear_velocity.x *= GROUND_FRICTION;
                            player_collider->base.linear_velocity.z *= GROUND_FRICTION;
                        }

                        player_collider->base.linear_velocity.y -= GRAVITY;
                    }
                }
            break;

            case PHY_COLLIDER_TYPE_PORTAL:
                portal_colliders = (struct portal_collider_t *)phy_colliders[type].buffer;

                c = phy_colliders[type].cursor;

                for(i = 0; i < c; i++)
                {
                    portal_collider = portal_colliders + i;

                    if(portal_collider->base.type != PHY_COLLIDER_TYPE_NONE)
                    {
                        phy_GenerateCollisionPairs(COLLIDER_HANDLE(PHY_COLLIDER_TYPE_PORTAL, i));
                    }
                }
            break;
        }
    }

    collision_pairs = (struct collision_pair_t *) phy_collision_pairs.buffer;

    c = phy_collision_pairs.cursor;

    for(i = 0; i < c; i++)
    {
        if(collision_pairs[i].collider_a.type == PHY_COLLIDER_TYPE_PLAYER &&
           collision_pairs[i].collider_b.type == PHY_COLLIDER_TYPE_PORTAL ||
           collision_pairs[i].collider_b.type == PHY_COLLIDER_TYPE_PLAYER &&
           collision_pairs[i].collider_a.type == PHY_COLLIDER_TYPE_PORTAL)
        {
            printf("COLLISION\n");
        }

        else if(collision_pairs[i].collider_a.type == PHY_COLLIDER_TYPE_PLAYER &&
                collision_pairs[i].collider_b.type == PHY_COLLIDER_TYPE_PLAYER)

        {
            printf("meh\n");
        }
    }
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
            collider_ptr->base.linear_velocity.y += 0.18;
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
        acceleration.x += collider_ptr->base.linear_velocity.x;
        acceleration.y = 0.0;
        acceleration.z += collider_ptr->base.linear_velocity.z;

        speed = acceleration.length();

        if(speed > MAX_HORIZONTAL_VELOCITY)
        {
            acceleration = normalize(acceleration) * MAX_HORIZONTAL_VELOCITY;
        }

        collider_ptr->base.linear_velocity.x = acceleration.x;
        collider_ptr->base.linear_velocity.z = acceleration.z;
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
    node_index = phy_dbvh_nodes.add(NULL);
    node = (struct dbvh_node_t *)phy_dbvh_nodes.get(node_index);

    node->parent = -1;
    node->children[0] = -1;
    node->children[1] = -1;
    node->collider = INVALID_COLLIDER_HANDLE;

    return node_index;
}

void phy_DeallocDbvhNode(int node_index)
{
    phy_dbvh_nodes.remove(node_index);
}

struct dbvh_node_t *phy_GetDbvhNodePointer(int node_index)
{
    return (struct dbvh_node_t *)phy_dbvh_nodes.get(node_index);
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

    //printf("-->%d\n", cur_node_index);

    if(phy_dbvh_root == -1)
    {
        /* the dbvh tree doesn't have a root,
        so set this node as the new root... */
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

    if(node->max.x >= cur_node->min.x && node->min.x <= cur_node->max.x)
    {
        if(node->max.y >= cur_node->min.y && node->min.y <= cur_node->max.y)
        {
            if(node->max.z >= cur_node->min.z && node->min.z <= cur_node->max.z)
            {
                if(cur_node->collider.index != INVALID_COLLIDER_INDEX)
                {
                    /* leaf node... */
                    pair.collider_a = node->collider;
                    pair.collider_b = cur_node->collider;
                    phy_collision_pairs.add(&pair);

                    //printf("COLLISION\n");
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

    //volume = (max.x - min.x) * (max.y - min.y) * (max.z - min.z);

    extents = max - min;

    /* smallest area heuristic... */
    area = ((extents.x * extents.z) +
            (extents.y * extents.z) +
            (extents.x * extents.y)) * 2.0;

    if(area < *smallest_area)
    {
        *smallest_area = area;
        *smallest_index = cur_node_index;
    }

    if(cur_node_index == phy_dbvh_root)
    {
        cur_node_index = *smallest_index;

        cur_node = phy_GetDbvhNodePointer(cur_node_index);

        if(node->parent == -1)
        {
            new_node_index = phy_AllocDbvhNode();
            new_node = phy_GetDbvhNodePointer(new_node_index);

            new_node->children[0] = node_index;
            new_node->children[1] = cur_node_index;

            if(cur_node->parent != -1)
            {
                parent = phy_GetDbvhNodePointer(cur_node->parent);
                parent->children[parent->children[0] != cur_node_index] = new_node_index;
                new_node->parent = cur_node->parent;
            }
            else
            {
                phy_dbvh_root = new_node_index;
            }

            node->parent = new_node_index;
            cur_node->parent = new_node_index;
        }
        else
        {
            if(node->parent == cur_node->parent)
            {
                return;
            }
            else
            {
                parent = phy_GetDbvhNodePointer(node->parent);
                sibling_index = parent->children[0] == node_index;
                sibling = phy_GetDbvhNodePointer(parent->children[sibling_index]);
                sibling->parent = parent->parent;

                if(parent->parent != -1)
                {
                    parent_parent = phy_GetDbvhNodePointer(parent->parent);
                    parent_parent->children[parent_parent->children[0] != node->parent] = parent->children[sibling_index];
                }
                else
                {
                    phy_dbvh_root = sibling->parent;
                }

                parent->children[sibling_index] = cur_node_index;


                parent_parent = phy_GetDbvhNodePointer(cur_node->parent);
                parent_parent->children[parent_parent->children[0] != cur_node_index] = node->parent;
                parent->parent = cur_node->parent;

                cur_node->parent = node->parent;
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
        if(node->collider.index == INVALID_COLLIDER_INDEX)
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
    struct portal_collider_t *portal_collider;
    struct portal_t *portal;

    float smallest_volume = FLT_MAX;
    int smallest_index = -1;

    vec2_t portal_size;
    vec3_t portal_corners[4];

    int i;
    int j;

    collider_ptr = phy_GetColliderPointer(collider);

    if(collider_ptr)
    {
        if(collider_ptr->dbvh_node == -1)
        {
            collider_ptr->dbvh_node = phy_AllocDbvhNode();
        }

        node = phy_GetDbvhNodePointer(collider_ptr->dbvh_node);
        node->collider = collider;

        switch(collider_ptr->type)
        {
            case PHY_COLLIDER_TYPE_PLAYER:
                player_collider = (struct player_collider_t *)collider_ptr;

                node->max = player_collider->base.position +
                            vec3_t(player_collider->radius,
                                   player_collider->height * 0.5,
                                   player_collider->radius);

                node->min = player_collider->base.position +
                            vec3_t(-player_collider->radius,
                                   -player_collider->height * 0.5,
                                   -player_collider->radius);
            break;

            case PHY_COLLIDER_TYPE_PORTAL:
                portal_collider = (struct portal_collider_t *)collider_ptr;
                portal = r_GetPortalPointer(portal_collider->portal_handle);

                node->max = vec3_t(-FLT_MAX, -FLT_MAX, -FLT_MAX);
                node->min = vec3_t(FLT_MAX, FLT_MAX, FLT_MAX);

                portal_size = portal->size * 0.5;

                portal_corners[0] = vec3_t(portal_size[0], portal_size[1], 0.0);
                portal_corners[1] = vec3_t(-portal_size[0], portal_size[1], 0.0);
                portal_corners[2] = vec3_t(-portal_size[0], -portal_size[1], 0.0);
                portal_corners[3] = vec3_t(portal_size[0], -portal_size[1], 0.0);

                for(i = 0; i < 4; i++)
                {
                    portal_corners[i] = portal_corners[i] * portal->orientation + portal->position;

                    for(j = 0; j < 3; j++)
                    {
                        if(portal_corners[i][j] > node->max[j]) node->max[j] = portal_corners[i][j];
                        if(portal_corners[i][j] < node->min[j]) node->min[j] = portal_corners[i][j];
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
        }

        phy_GenerateCollisionPairsRecursive(phy_dbvh_root, collider_ptr->dbvh_node, &smallest_volume, &smallest_index);
        phy_RecomputeVolumesRecursive(collider_ptr->dbvh_node);
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


void phy_BuildBvh(vec3_t *vertices, int vertice_count)
{
    int i;
    int j;

    struct bvh_node_t *nodes = NULL;
    struct bvh_node_t *new_node = NULL;

    vertice_count -= vertice_count % 3;

    phy_collision_triangle_count = 0;

    if(vertice_count)
    {
        phy_collision_triangles = (struct collision_triangle_t *)calloc(sizeof(collision_triangle_t), vertice_count / 3);

        for(i = 0; i < vertice_count;)
        {
            phy_collision_triangles[phy_collision_triangle_count].normal = normalize(cross(vertices[i + 2] - vertices[i + 1], vertices[i + 1] - vertices[i]));

            new_node = (struct bvh_node_t *)calloc(sizeof(struct bvh_node_t ), 1);
            new_node->left = NULL;
            new_node->right = NULL;

            new_node->max = vec3_t(-FLT_MAX, -FLT_MAX, -FLT_MAX);
            new_node->min = vec3_t(FLT_MAX, FLT_MAX, FLT_MAX);

            for(j = 0; j < 3; j++, i++)
            {
                phy_collision_triangles[phy_collision_triangle_count].verts[j] = vertices[i];

                if(vertices[i].x > new_node->max.x) new_node->max.x = vertices[i].x;
                if(vertices[i].x < new_node->min.x) new_node->min.x = vertices[i].x;

                if(vertices[i].y > new_node->max.y) new_node->max.y = vertices[i].y;
                if(vertices[i].y < new_node->min.y) new_node->min.y = vertices[i].y;

                if(vertices[i].z > new_node->max.z) new_node->max.z = vertices[i].z;
                if(vertices[i].z < new_node->min.z) new_node->min.z = vertices[i].z;
            }

            for(j = 0; j < 3; j++)
            {
                if(fabs(new_node->max[j] - new_node->min[j]) < 0.001)
                {
                    new_node->max[j] += 0.05;
                    new_node->min[j] -= 0.05;
                }
            }

            new_node->data = phy_collision_triangles + phy_collision_triangle_count;


            new_node->parent = nodes;
            nodes = new_node;

            phy_collision_triangle_count++;
        }

        phy_collision_bvh = phy_BuildBvhRecursive(nodes);
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
struct collision_triangle_t *collision_triangles_buffer[MAX_COLLISION_TRIANGLES];
struct list_t collision_triangles;

struct list_t *phy_BoxOnBvh(vec3_t &aabb_max, vec3_t &aabb_min)
{
    if(phy_collision_bvh)
    {
        collision_triangles.buffer = collision_triangles_buffer;
        collision_triangles.cursor = 0;
        collision_triangles.elem_size = sizeof(struct collision_triangle_t **);
        collision_triangles.size = sizeof(struct collision_triangle_t **) * MAX_COLLISION_TRIANGLES;

        phy_BoxOnBvhRecursive(aabb_max, aabb_min, phy_collision_bvh, &collision_triangles);

        return &collision_triangles;
    }

    return NULL;
}

void phy_SolveContactPoints()
{

}

#define MAX_CONTACT_POINTS 2048
struct contact_point_t contact_points_buffer[MAX_CONTACT_POINTS];
struct list_t contact_points;

struct list_t *phy_FindContactPointsPlayerWorld(struct collider_handle_t player_collider)
{
    int i;
    int j;
    struct player_collider_t *collider;
    struct list_t *triangles;
    struct collision_triangle_t *triangle;
    struct contact_point_t contact;
    vec3_t collider_top;
    vec3_t capsule_vec;
    vec3_t closest_on_capsule;
    vec3_t closest_on_plane;
    vec3_t closest_on_triangle;

    vec3_t planes[3];
    vec3_t edge;
    vec3_t dists;

    vec3_t baricenter;

    vec3_t aabb_max;
    vec3_t aabb_min;

    vec3_t col_tri_vec;
    float col_t;
    float edge_t;
    float tri_dist;

    float area;

    collider = phy_GetPlayerColliderPointer(player_collider);

    struct view_t *view = r_GetActiveView();

    if(collider)
    {
        contact_points.buffer = contact_points_buffer;
        contact_points.cursor = 0;
        contact_points.elem_size = sizeof(struct contact_point_t);
        contact_points.size = sizeof(struct contact_point_t) * MAX_CONTACT_POINTS;

        collider_top = collider->base.position + vec3_t(0.0, collider->height * 0.5 - collider->radius, 0.0);
        capsule_vec = (collider->base.position - vec3_t(0.0, collider->height * 0.5 - collider->radius, 0.0)) - collider_top;

        aabb_max = collider_top + vec3_t(collider->radius, collider->radius, collider->radius);
        aabb_min = collider_top + capsule_vec - vec3_t(collider->radius, collider->radius, collider->radius);

        triangles = phy_BoxOnBvh(aabb_max, aabb_min);

        for(i = 0; i < triangles->cursor; i++)
        {
            triangle = *(struct collision_triangle_t **)triangles->get(i);

            col_tri_vec = triangle->verts[0] - collider_top;
            col_t = dot(triangle->normal, col_tri_vec) / dot(triangle->normal, capsule_vec);

            if(col_t > 1.0)
            {
                col_t = 1.0;
            }
            else if(col_t < 0.0)
            {
                col_t = 0.0;
            }

            closest_on_capsule = collider_top + capsule_vec * col_t;

            /* project the closest point to the capsule onto the triangle's plane... */
            closest_on_plane = closest_on_capsule - triangle->normal * dot(closest_on_capsule - triangle->verts[0], triangle->normal);

            /* build the edge planes' normals... */
            planes[0] = cross(triangle->normal, triangle->verts[1] - triangle->verts[0]);
            planes[1] = cross(triangle->normal, triangle->verts[2] - triangle->verts[1]);
            planes[2] = cross(triangle->normal, triangle->verts[0] - triangle->verts[2]);

            /* calculate the distance between the projected point and the edge planes... */
            dists[0] = dot(closest_on_plane - triangle->verts[0], planes[0]) - collider->radius;
            dists[1] = dot(closest_on_plane - triangle->verts[1], planes[1]) - collider->radius;
            dists[2] = dot(closest_on_plane - triangle->verts[2], planes[2]) - collider->radius;


            if(dists[0] > 0.0 || dists[1] > 0.0 || dists[2] > 0.0)
            {
                /* the closest point on the triangle's plane has a positive distance
                to one of the edge planes, which means the point on the plane lies
                outside the triangle... */

                /* find to which edge the point on the plane is closer to...*/
                if(dists[0] > dists[1])
                {
                    if(dists[0] > dists[2])
                    {
                        j = 0;
                    }
                    else
                    {
                        j = 2;
                    }
                }
                else
                {
                    if(dists[1] > dists[2])
                    {
                        j = 1;
                    }
                    else
                    {
                        j = 2;
                    }
                }

                edge = triangle->verts[(j + 1) % 3] - triangle->verts[j];

                /* project the point in the plane onto the closest edge... */
                edge_t = dot(closest_on_plane - triangle->verts[j], edge) / dot(edge, edge);

                /* make sure the projection lies within the edge... */
                if(edge_t > 1.0)
                {
                    edge_t = 1.0;
                }
                else if(edge_t < 0.0)
                {
                    edge_t = 0.0;
                }

                closest_on_plane = triangle->verts[j] + edge * edge_t;
            }

            col_tri_vec = closest_on_capsule - closest_on_plane;
            tri_dist = length(col_tri_vec);

            if(tri_dist < collider->radius)
            {
                if(tri_dist < 0.0001)
                {
                    /* HACK: sometimes, due to numeric precision, a intersection
                    will generate a contact point with a normal opposite the triangle
                    normal. This happens when the capsule is going through the triangle,
                    and instead of having distance and vector zero, will have a vector
                    pointing slightly in the opposite direction of the triangle's normal... */
                    col_tri_vec = triangle->normal;
                }
                else
                {
                    col_tri_vec /= tri_dist;
                }

                contact.position = closest_on_plane;
                contact.normal = col_tri_vec;
                contact.penetration = collider->radius - tri_dist;
                contact_points.add(&contact);
            }
        }

        return &contact_points;
    }

    return NULL;
}

struct list_t *phy_FindContactPointsPlayerPortal(struct collider_handle_t player_collider, struct collider_handle_t portal_collider)
{

}

#define MAX_ITERATIONS 8

void phy_CollidePlayerWorld(struct collider_handle_t player_collider)
{
    struct list_t *contacts;
    int i;
    int j;
    int deepest_index;
    struct contact_point_t *contact;
    struct player_collider_t *collider;

    vec3_t pos_correction;
    vec3_t vel_correction;

    /* hmm... */
    float deepest;


    collider = phy_GetPlayerColliderPointer(player_collider);

    if(collider)
    {
        collider->flags &= ~PHY_PLAYER_COLLIDER_FLAG_ON_GROUND;

        for(j = 0; j < MAX_ITERATIONS; j++)
        {
            contacts = phy_FindContactPointsPlayerWorld(player_collider);
            pos_correction = vec3_t(0.0, 0.0, 0.0);

            deepest = -FLT_MAX;

            if(contacts->cursor)
            {
                for(i = 0; i < contacts->cursor; i++)
                {
                    contact = (struct contact_point_t *)contacts->get(i);

                    if(contact->penetration > deepest)
                    {
                        deepest = contact->penetration;
                        deepest_index = i;
                    }
//                    contact = (struct contact_point_t *)contacts->get(i);
//                    pos_correction += contact->normal * contact->penetration;
//
//                    if(dot(contact->normal, vec3_t(0.0, 1.0, 0.0)) > 0.7)
//                    {
//                        collider->flags &= ~PHY_PLAYER_COLLIDER_FLAG_FLYING;
//                    }
//
//                    if(dot(collider->base.linear_velocity, contact->normal) < 0.0)
//                    {
//                        /* clip the linear velocity to make it go parallel to the contact plane... */
//                        collider->base.linear_velocity = collider->base.linear_velocity - contact->normal *
//                                                    dot(collider->base.linear_velocity, contact->normal);
//                    }
                }

                contact = (struct contact_point_t *)contacts->get(deepest_index);

                if(dot(contact->normal, vec3_t(0.0, 1.0, 0.0)) > 0.7)
                {
                    collider->flags |= PHY_PLAYER_COLLIDER_FLAG_ON_GROUND;
                }

                if(dot(collider->base.linear_velocity, contact->normal) < 0.0)
                {
                    /* clip the linear velocity to make it go parallel to the contact plane... */
                    collider->base.linear_velocity = collider->base.linear_velocity - contact->normal *
                                                dot(collider->base.linear_velocity, contact->normal);
                }

                collider->base.position += contact->normal * contact->penetration;

                //pos_correction /= contacts->cursor;

                //collider->base.position += pos_correction;
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

}

#ifdef __cplusplus
}
#endif








