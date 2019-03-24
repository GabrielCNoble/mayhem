#include "physics.h"
#include "stack_list.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "r_common.h"
#include "r_view.h"

#include <stdio.h>

#define GRAVITY 0.009
#define MAX_HORIZONTAL_VELOCITY 0.15
#define GROUND_FRICTION 0.9



struct stack_list_t phy_colliders[PHY_COLLIDER_TYPE_NONE];

struct bvh_node_t *phy_collision_bvh = NULL;
int phy_collision_triangle_count = 0;
struct collision_triangle_t *phy_collision_triangles = NULL;

#ifdef __cplusplus
extern "C"
{
#endif


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

                    phy_CollidePlayerWorld(COLLIDER_HANDLE(PHY_COLLIDER_TYPE_PLAYER, i));

                    if(!(player_collider->flags & PHY_PLAYER_COLLIDER_FLAG_FLYING))
                    {
                        player_collider->base.linear_velocity.x *= GROUND_FRICTION;
                        player_collider->base.linear_velocity.z *= GROUND_FRICTION;
                    }

                    //phy_FindContactPointsPlayerWorld(COLLIDER_HANDLE(PHY_COLLIDER_TYPE_PLAYER, i));

//                    if(player_collider->base.position.y - player_collider->height * 0.5 < 0.0)
//                    {
//                        player_collider->base.position.y = player_collider->height * 0.5;
//                        player_collider->base.linear_velocity.y = 0.0;
//
//                        player_collider->flags &= ~PHY_PLAYER_COLLIDER_FLAG_FLYING;
//                    }
//
//                    if(!(player_collider->flags & PHY_PLAYER_COLLIDER_FLAG_FLYING))
//                    {
//                        player_collider->base.linear_velocity.x *= GROUND_FRICTION;
//                        player_collider->base.linear_velocity.z *= GROUND_FRICTION;
//                    }

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

        if(collider_ptr && !(collider_ptr->flags & PHY_PLAYER_COLLIDER_FLAG_FLYING))
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
            new_node->triangle = NULL;
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
//                if(new_node->max[j] == 0.0) new_node->max[j] = 0.01;
//                if(new_node->min[j] == 0.0) new_node->min[j] = -0.01;
            }

            new_node->triangle = phy_collision_triangles + phy_collision_triangle_count;


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

    if(node->triangle)
    {
        triangle_list->add(&node->triangle);
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

                dists[0] = dists[0];
                dists[1] = dists[1];
                dists[2] = dists[2];

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

                dists = vec3_t(-1.0, -1.0, -1.0);
            }



            //if(dists[0] <= 0.0 && dists[1] <= 0.0 && dists[2] <= 0.0)
            //{

            col_tri_vec = closest_on_capsule - closest_on_plane;
            tri_dist = length(col_tri_vec);

            if(tri_dist == 0.0)
            {
                col_tri_vec = triangle->normal;
            }
            else
            {
                col_tri_vec /= tri_dist;
            }

            if(tri_dist < collider->radius)
            {
                contact.position = closest_on_plane;
                contact.normal = col_tri_vec;
                contact.penetration = collider->radius - tri_dist;
                contact_points.add(&contact);
            }

            //}

            //phy_closest_point = closest_on_plane;
        }

        return &contact_points;
    }

    return NULL;
}

void phy_CollidePlayerWorld(struct collider_handle_t player_collider)
{
    struct list_t *contacts;
    int i;
    struct contact_point_t *contact;
    struct player_collider_t *collider;

    vec3_t pos_correction;
    vec3_t vel_correction;


    collider = phy_GetPlayerColliderPointer(player_collider);

    if(collider)
    {
        contacts = phy_FindContactPointsPlayerWorld(player_collider);
        pos_correction = vec3_t(0.0, 0.0, 0.0);
        vel_correction = vec3_t(0.0, 0.0, 0.0);

        collider->flags |= PHY_PLAYER_COLLIDER_FLAG_FLYING;

        if(contacts->cursor)
        {
            glBegin(GL_POINTS);
            glColor3f(1.0, 0.0, 0.0);

            for(i = 0; i < contacts->cursor; i++)
            {
                contact = (struct contact_point_t *)contacts->get(i);
                pos_correction += contact->normal * contact->penetration;

                if(dot(contact->normal, vec3_t(0.0, 1.0, 0.0)) > 0.7)
                {
                    collider->flags &= ~PHY_PLAYER_COLLIDER_FLAG_FLYING;
                }

                if(dot(collider->base.linear_velocity, contact->normal) < 0.0)
                {
                    /* clip the linear velocity to make it go parallel to the contact plane... */
                    collider->base.linear_velocity = collider->base.linear_velocity - contact->normal *
                                                dot(collider->base.linear_velocity, contact->normal);
                }

                glVertex3f(contact->position.x, contact->position.y, contact->position.z);
            }

            glEnd();

          //  printf("[%f %f %f]\n", pos_correction.x, pos_correction.y, pos_correction.z);

            pos_correction /= contacts->cursor;

            collider->base.position += pos_correction;
        }
    }

}

#ifdef __cplusplus
}
#endif








