#include "geometry.h"
#include <math.h>
#include "../common/r_dbg.h"
//#include <stdio.h>

vec3_t support_vertex_upright_capsule(const vec3_t &dir, float height, float radius, vec3_t &normal)
{
    vec3_t h;
    vec3_t d;
    vec3_t point;
    float c;
    float s;

    s = height * 0.5 - radius;
    point.y = dir.y * height * 0.5;

    if(fabs(point.y) <= s)
    {
        h.x = dir.x;
        h.z = dir.z;
        h.y = 0.0;
        h = normalize(h);
        normal = h;
        point.x = h.x * radius;
        point.z = h.z * radius;
    }
    else
    {
        d.x = dir.x;
        d.y = point.y;
        d.z = dir.z;
        c = d.y > 0.0 ? s : -s;
        d.y -= c;
        d = normalize(d);
        normal = d;
        point.x = d.x * radius;
        point.y = d.y * radius + c;
        point.z = d.z * radius;
    }

    return point;
}

vec3_t support_vertex_triangle(const vec3_t &dir, const vec3_t &a, const vec3_t &b, const vec3_t &c)
{
    vec3_t planes[3];
    vec3_t barycenter;
    vec3_t vert;
    vec3_t point;
    vec3_t triangle_normal;
    vec3_t dir_on_plane;

    float d_a;
    float d_b;
    int i;

    float max_d;
    float d;
    int max_i;

    triangle_normal = normalize(cross(b - a, c - b));
    barycenter = (a + b + c) / 3.0;

//    if(dot(triangle_normal, dir) == -1.0)
//    {
//        point = barycenter;
//        normal = triangle_normal;
//        return;
//    }

    planes[0] = cross(normalize(b - a), triangle_normal);
    planes[1] = cross(normalize(c - b), triangle_normal);
    planes[2] = cross(normalize(a - c), triangle_normal);

    max_d = 0.0;
    max_i = 0;

    for(i = 0; i < 3; i++)
    {
        d = dot(dir, planes[i]);

        if(d > max_d)
        {
            max_d = d;
            max_i = i;
        }
    }

    switch(max_i)
    {
        case 0:
            vert = a;
        break;

        case 1:
            vert = b;
        break;

        case 2:
            vert = c;
        break;
    }
    dir_on_plane = dir - triangle_normal * dot(dir, triangle_normal);
    d_a = dot(planes[max_i], vert - (barycenter + dir_on_plane));
    point = barycenter + dir_on_plane * d_a;

    return point;
}

vec3_t closest_point_on_triangle(const vec3_t &point, const vec3_t &a, const vec3_t &b, const vec3_t &c)
{
//    float col_t;
    float edge_t;
    float tri_dist;
    int j;

    vec3_t tri_vec;
    vec3_t projected;
    vec3_t planes[3];
    vec3_t edge;
    vec3_t vert;
    vec3_t normal;
    float dists[3];

    normal = normalize(cross(c - b, b - a));
    projected = point - normal * dot(normal, point - a);

    /* build the edge planes' normals... */
    planes[0] = cross(normal, b - a);
    planes[1] = cross(normal, c - b);
    planes[2] = cross(normal, a - c);

    /* calculate the distance between the projected point and the edge planes... */
    dists[0] = dot(projected - a, planes[0]);
    dists[1] = dot(projected - b, planes[1]);
    dists[2] = dot(projected - c, planes[2]);

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
                vert = a;
                edge = b - a;
            }
            else
            {
                vert = c;
                edge = a - c;
            }
        }
        else
        {
            if(dists[1] > dists[2])
            {
                vert = b;
                edge = c - b;
            }
            else
            {
                vert = c;
                edge = a - c;
            }
        }

        /* project the point in the plane onto the closest edge... */
        edge_t = dot(projected - vert, edge) / dot(edge, edge);

        /* make sure the projection lies within the edge... */
        if(edge_t > 1.0)
        {
            edge_t = 1.0;
        }
        else if(edge_t < 0.0)
        {
            edge_t = 0.0;
        }

        projected = vert + edge * edge_t;
    }

    return projected;
}

void closest_point_line_triangle(const vec3_t &v0, const vec3_t &v1, const vec3_t &v2, const vec3_t &a, const vec3_t &b, vec3_t *point_on_tri, vec3_t *point_on_line)
{
    float edge_t;
    float a_dist;
    float b_dist;

    vec3_t planes[3];
    vec3_t edge;
    vec3_t vert;
    vec3_t triangle_normal;
    vec3_t dists;
    vec3_t closest;
    vec3_t projected;
    vec3_t line_vec;

    triangle_normal = normalize(cross(v2 - v1, v1 - v0));

    /* build the edge planes. The normals point outwards,
    which means that a point that is inside the triangle
    has negative distance... */
    planes[0] = cross(triangle_normal, v1 - v0);
    planes[1] = cross(triangle_normal, v2 - v1);
    planes[2] = cross(triangle_normal, v0 - v2);

    a_dist = dot(a - v0, triangle_normal);
    b_dist = dot(b - v0, triangle_normal);

    if(b_dist != 0.0 && a_dist != b_dist)
    {
        edge_t = a_dist / (a_dist - b_dist);
    }
    else
    {
        /* line is perfectly parallel to plane... */
        edge_t = 0.0;
    }

    if(edge_t < 0.0)
    {
        edge_t = 0.0;
    }
    else if(edge_t > 1.0)
    {
        edge_t = 1.0;
    }

    line_vec = b - a;

    /* this handles the case of endpoints being inside the
    triangle. If the line intersects the triangle, 'closest'
    will be the intersection point. Otherwise, it'll be one
    of the endpoints... */
    closest = a + line_vec * edge_t;

    /* compute the distance between 'closest' and
    the edge planes... */
    dists.x = dot(closest - v0, planes[0]);
    dists.y = dot(closest - v1, planes[1]);
    dists.z = dot(closest - v2, planes[2]);

    /* project the closest point onto the triangle's plane. The point
    may be outside the triangle... */
    projected = closest - triangle_normal * dot(closest - v0, triangle_normal);

    if(dists.x < 0.0 && dists.y < 0.0 && dists.z < 0.0)
    {
        /* endpoints are inside the triangle, which means
        the project point is also inside the triangle... */
        *point_on_tri = projected;
        *point_on_line = closest;
        return;
    }

    /* one of the endpoints is outside the triangle... */


    /* find which edge the projected closest point is closer to... */
    if(dists.x > dists.y)
    {
        if(dists.x > dists.z)
        {
            vert = v0;
            edge = v1 - v0;
        }
        else
        {
            vert = v2;
            edge = v0 - v2;
        }
    }
    else
    {
        if(dists.y > dists.z)
        {
            vert = v1;
            edge = v2 - v1;
        }
        else
        {
            vert = v2;
            edge = v0 - v2;
        }
    }

    edge_t = dot(projected - vert, edge) / dot(edge, edge);

    /* clamp 'edge_t' so the projected closest point lies within the edge when
    reprojected... */
    if(edge_t < 0.0)
    {
        edge_t = 0.0;
    }
    else if(edge_t > 1.0)
    {
        edge_t = 1.0;
    }

    /* reproject the projected closest point onto the closest edge, to
    bring it to the inside of the triangle... */
    projected = vert + edge * edge_t;

    edge_t = dot(projected - a, line_vec) / dot(line_vec, line_vec);

    if(edge_t < 0.0)
    {
        edge_t = 0.0;
    }
    else if(edge_t > 1.0)
    {
        edge_t = 1.0;
    }

    /* project the reprojected closest point onto the original line, to find the
    closest point on the line that also projects inside the triangle... */
    closest = a + line_vec * edge_t;

    *point_on_tri = projected;
    *point_on_line = closest;
}





