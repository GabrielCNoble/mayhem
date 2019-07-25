#include "geo.h"

#include <math.h>

void geo_ComputeTangents(vec3_t *vertices, vec2_t *tex_coords, vec3_t *tangents, int vertice_count)
{
    int i;

    vec3_t tangent;

    for(i = 0; i < vertice_count; i += 3)
    {
        tangent = normalize(vertices[i + 1] - vertices[i]);
        tangents[i    ] = tangent;
        tangents[i + 1] = tangent;
        tangents[i + 2] = tangent;
    }
}

vec3_t geo_SupportVertexUprightCapsule(vec3_t dir, float height, float radius)
{
    vec3_t support;
    vec3_t h;
    float c;
    float s;

    s = height * 0.5 - radius;
    support.y = dir.y * height * 0.5;

    if(fabs(support.y) <= s)
    {
        h.x = dir.x;
        h.z = dir.z;
        h.y = 0.0;
        h = normalize(h);

        support.x = h.x * radius;
        support.z = h.z * radius;
    }
    else
    {
        c = dir.y < 0.0 ? -s : s;
        dir.y -= c;

        dir = normalize(dir);

        support.x = dir.x * radius;
        support.z = dir.z * radius;
        support.y = dir.y * radius + c;
    }

    return support;
}
