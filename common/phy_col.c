#include "phy_col.h"
#include "r_dbg.h"
#include "../utils/geo.h"
#include "../gmath/geometry.h"

#include <stdio.h>
#include <math.h>



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

int phy_ContactCapsuleTriangle(struct collision_triangle_t *triangle, struct capsule_shape_t *capsule_shape, vec3_t &position, vec3_t &velocity, struct contact_point_t *contact)
{
    vec3_t capsule_point;
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
                                    &triangle_point, &capsule_point);

    capsule_point = capsule_point + normalize(triangle_point - capsule_point) * capsule_shape->radius;
    dir = normalize(capsule_point - triangle_point);

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

int phy_ContactSphereTriangle(struct collision_triangle_t *triangle, struct sphere_shape_t *sphere_shape, vec3_t &position, vec3_t &velocity, struct contact_point_t *contact)
{
    vec3_t sphere_point;
    vec3_t triangle_point;
    vec3_t point;
    vec3_t normal;
    vec3_t dir;
    float col_t;
    float a;
    float b;
    int i;

//    r_DrawTriangle(triangle->verts[0], triangle->verts[1], triangle->verts[2], vec3_t(0.0, 1.0, 0.0), 1, 0, 1);

    triangle_point = closest_point_on_triangle(position, triangle->verts[0], triangle->verts[1], triangle->verts[2]);
//    r_DrawPoint(triangle_point, vec3_t(1.0, 0.0, 0.0), 8.0, 1);
    sphere_point = position + normalize(triangle_point - position) * sphere_shape->radius;
    dir = normalize(sphere_point - triangle_point);

    b = dot(dir, (sphere_point + velocity) - triangle_point);

    if(b != 0.0)
    {
        a = dot(dir, sphere_point - triangle_point);
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

int phy_ContactBoxBox(struct box_shape_t *box_shape_a, vec3_t &position_a, struct box_shape_t *box_shape_b, vec3_t &position_b, struct contact_point_t *contact)
{

}

int phy_ContactCapsuleBox(struct box_shape_t *box_shape, vec3_t &position_box, struct capsule_shape_t *capsule, vec3_t &position_capsule, struct contact_point_t *contact)
{

}
