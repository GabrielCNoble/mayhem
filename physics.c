#include "physics.h"
#include "stack_list.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include <stdio.h>

#define GRAVITY 0.0098
#define MAX_HORIZONTAL_VELOCITY 2.0
#define GROUND_FRICTION 0.9



struct stack_list_t phy_colliders[PHY_COLLIDER_TYPE_NONE];

struct bvh_node_t *phy_collision_bvh = NULL;
int phy_collision_triangle_count = 0;
struct collision_triangle_t *phy_collision_triangles = NULL;



void phy_Init()
{
    phy_colliders[PHY_COLLIDER_TYPE_PLAYER].init(sizeof(struct player_collider_t), 32);
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
    }

    return handle;
}

struct base_collider_t *phy_GetColliderPointer(struct collider_handle_t collider)
{
    struct base_collider_t *collider_ptr = NULL;

    if(collider.type >= PHY_COLLIDER_TYPE_PLAYER && collider.type < PHY_COLLIDER_TYPE_PROJECTILE)
    {
        if(collider.index != INVALID_COLLIDER_INDEX)
        {
            if(collider.index < phy_colliders[collider.type].cursor)
            {
                collider_ptr = (struct base_collider_t *)phy_colliders[collider.type].get(collider.index);

                if(collider.type != collider_ptr->type)
                {
                    collider_ptr = NULL;
                }
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


void phy_UpdateColliders()
{
    int type;
    int i;
    int c;
    struct player_collider_t *player_colliders;
    struct player_collider_t *player_collider;

    for(type = PHY_COLLIDER_TYPE_PLAYER; type < PHY_COLLIDER_TYPE_PROJECTILE; type++)
    {
        switch(type)
        {
            case PHY_COLLIDER_TYPE_PLAYER:
                player_colliders = (struct player_collider_t *)phy_colliders[type].buffer;

                c = phy_colliders[type].cursor;

                for(i = 0; i < c; i++)
                {
                    player_collider = player_colliders + i;

                    player_collider->base.position += player_collider->base.linear_velocity;

                    if(player_collider->base.position.y - player_collider->height * 0.5 < 0.0)
                    {
                        player_collider->base.position.y = player_collider->height * 0.5;
                        player_collider->base.linear_velocity.y = 0.0;

                        player_collider->flags &= ~PHY_PLAYER_COLLIDER_FLAG_FLYING;
                    }

                    if(!(player_collider->flags & PHY_PLAYER_COLLIDER_FLAG_FLYING))
                    {
                        player_collider->base.linear_velocity.x *= GROUND_FRICTION;
                        player_collider->base.linear_velocity.z *= GROUND_FRICTION;
                    }

                    player_collider->base.linear_velocity.y -= GRAVITY;
                }
            break;
        }
    }
}


void phy_Jump(struct collider_handle_t collider)
{
    struct player_collider_t *collider_ptr;

    if(collider.type == PHY_COLLIDER_TYPE_PLAYER)
    {
        collider_ptr = (struct player_collider_t *)phy_GetColliderPointer(collider);

        if(collider_ptr)
        {
            collider_ptr->base.linear_velocity.y += 0.5;
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
                if(new_node->max[j] == 0.0) new_node->max[j] = 0.005;
                if(new_node->min[j] == 0.0) new_node->min[j] = -0.005;
            }

            new_node->triangle = phy_collision_triangles + phy_collision_triangle_count;


            new_node->parent = nodes;
            nodes = new_node;

            phy_collision_triangle_count++;
        }

        phy_collision_bvh = phy_BuildBvhRecursive(nodes);
    }
}


void phy_TraverseBvhRecursive(vec3_t &aabb_max, vec3_t &aabb_min, struct bvh_node_t *node, struct list_t *triangle_list)
{
    struct bvh_node_t *child;
    vec3_t box_center;
    vec3_t box_extents;
    vec3_t box_triangle_vec;
    int i;


    child = node->left;

    for(i = 0; i < 2; i++)
    {
        if(aabb_max.x >= child->min.x && aabb_min.x <= child->max.x)
        {
            if(aabb_max.y >= child->min.y && aabb_min.y <= child->max.y)
            {
                if(aabb_max.z >= child->min.z && aabb_min.z <= child->max.z)
                {
                    if(node->triangle)
                    {
                        triangle_list->add(node->triangle);
                    }
                    else
                    {
                        phy_TraverseBvhRecursive(aabb_max, aabb_min, child, triangle_list);
                    }

                    return;
                }
            }
        }

        child = node->right;
    }

}


#define MAX_COLLISION_TRIANGLES 2048
struct collision_triangle_t *collision_triangles_buffer[MAX_COLLISION_TRIANGLES];
struct list_t collision_triangles;

struct list_t *phy_TraverseBvh(vec3_t &aabb_max, vec3_t &aabb_min)
{
    if(phy_collision_bvh)
    {
        collision_triangles.buffer = collision_triangles_buffer;
        collision_triangles.cursor = 0;
        collision_triangles.elem_size = sizeof(struct collision_triangle_t *);
        collision_triangles.size = sizeof(struct collision_triangle_t *) * MAX_COLLISION_TRIANGLES;

        phy_TraverseBvhRecursive(aabb_max, aabb_min, phy_collision_bvh, &collision_triangles);

        return &collision_triangles;
    }

    return NULL;
}







