#include "physics.h"
#include "stack_list.h"
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#define GRAVITY 0.0098
#define MAX_HORIZONTAL_VELOCITY 2.0
#define GROUND_FRICTION 0.9



struct stack_list_t colliders[PHY_COLLIDER_TYPE_NONE];

struct bvh_node_t *collision_bvh = NULL;
int collision_triangle_count = 0;
struct collision_triangle_t *collision_triangles = NULL;



void phy_Init()
{
    colliders[PHY_COLLIDER_TYPE_PLAYER].init(sizeof(struct player_collider_t), 32);
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
        handle.index = colliders[type].add(NULL);

        collider = (struct base_collider_t *)colliders[type].get(handle.index);
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
            if(collider.index < colliders[collider.type].cursor)
            {
                collider_ptr = (struct base_collider_t *)colliders[collider.type].get(collider.index);

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
                player_colliders = (struct player_collider_t *)colliders[type].buffer;

                c = colliders[type].cursor;

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


void phy_ClearBvh()
{

}


void phy_BuildBvh(vec3_t *vertices, int vertice_count)
{
    int i;

    vertice_count -= vertice_count % 3;

    collision_triangle_count = 0;

    if(vertice_count)
    {
        collision_triangles = (struct collision_triangle_t *)calloc(sizeof(collision_triangle_t), vertice_count / 3);

        for(i = 0; i < vertice_count;)
        {
            collision_triangles[collision_triangle_count].normal = cross(vertices[i + 2] - vertices[i + 1], vertices[i + 1] - vertices[i]);

            collision_triangles[collision_triangle_count].verts[0] = vertices[i];
            i++;
            collision_triangles[collision_triangle_count].verts[1] = vertices[i];
            i++;
            collision_triangles[collision_triangle_count].verts[2] = vertices[i];
            i++;

            collision_triangle_count++;
        }
    }
}









