#include "phy_col.h"

#include <stdio.h>
#include <math.h>

#define MIN_CONTACT_PENETRATION 0.0001


int phy_ContactBoxTriangle(struct collision_triangle_t *triangle, struct box_shape_t *box_shape, vec3_t &position, mat3_t &orientation, struct contact_point_t *contact)
{
    vec3_t static_rigid_vec;
    vec3_t closest_on_plane;
    float dist;
    float radius;
    vec3_t planes;

    static_rigid_vec = position - triangle->verts[0];

    closest_on_plane = position - triangle->normal * dot(static_rigid_vec, triangle->normal);
    static_rigid_vec = position - closest_on_plane;

    dist = length(static_rigid_vec);
    static_rigid_vec /= dist;

    radius = box_shape->size[0] * 0.5 * fabs(dot(orientation[0], triangle->normal)) +
             box_shape->size[1] * 0.5 * fabs(dot(orientation[1], triangle->normal)) +
             box_shape->size[2] * 0.5 * fabs(dot(orientation[2], triangle->normal));

    if(dist < radius)
    {
        //printf("COLLISION!\n");
    }

}

int phy_ContactCapsuleTriangle(struct collision_triangle_t *triangle, struct capsule_shape_t *capsule_shape, vec3_t &position, struct contact_point_t *contact)
{
    vec3_t capsule_top = position + vec3_t(0.0, capsule_shape->height * 0.5, 0.0);
    vec3_t capsule_vec = vec3_t(0.0, -capsule_shape->height, 0.0);
    vec3_t closest_on_plane;
    vec3_t closest_on_capsule;
    vec3_t col_tri_vec;

    float col_t;
    float edge_t;
    float tri_dist;
    int j;

    vec3_t planes[3];
    vec3_t edge;
    float dists[3];

    col_tri_vec = triangle->verts[0] - capsule_top;
    col_t = dot(triangle->normal, col_tri_vec) / dot(triangle->normal, capsule_vec);

    if(col_t > 1.0)
    {
        col_t = 1.0;
    }
    else if(col_t < 0.0)
    {
        col_t = 0.0;
    }

    closest_on_capsule = capsule_top + capsule_vec * col_t;

    /* project the closest point to the capsule onto the triangle's plane... */
    closest_on_plane = closest_on_capsule - triangle->normal * dot(closest_on_capsule - triangle->verts[0], triangle->normal);
    //printf("[%f %f %f]\n", triangle->normal[0], triangle->normal[1], triangle->normal[2]);

    /* build the edge planes' normals... */
    planes[0] = cross(triangle->normal, triangle->verts[1] - triangle->verts[0]);
    planes[1] = cross(triangle->normal, triangle->verts[2] - triangle->verts[1]);
    planes[2] = cross(triangle->normal, triangle->verts[0] - triangle->verts[2]);

    /* calculate the distance between the projected point and the edge planes... */
    dists[0] = dot(closest_on_plane - triangle->verts[0], planes[0]) - capsule_shape->radius;
    dists[1] = dot(closest_on_plane - triangle->verts[1], planes[1]) - capsule_shape->radius;
    dists[2] = dot(closest_on_plane - triangle->verts[2], planes[2]) - capsule_shape->radius;


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

    contact->penetration = 0.0;

    if(tri_dist < capsule_shape->radius)
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

        contact->position = closest_on_plane;
        contact->normal = col_tri_vec;
        contact->surface_normal = triangle->normal;
        contact->penetration = capsule_shape->radius - tri_dist;
    }

    return contact->penetration > MIN_CONTACT_PENETRATION;
}

int phy_ContactBoxBox(struct box_shape_t *box_shape_a, vec3_t &position_a, struct box_shape_t *box_shape_b, vec3_t &position_b, struct contact_point_t *contact)
{

}

int phy_ContactCapsuleBox(struct box_shape_t *box_shape, vec3_t &position_box, struct capsule_shape_t *capsule, vec3_t &position_capsule, struct contact_point_t *contact)
{

}
