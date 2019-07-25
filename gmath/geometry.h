#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "vector.h"

vec3_t support_vertex_upright_capsule(const vec3_t &dir, float height, float radius, vec3_t &normal);

vec3_t support_vertex_triangle(const vec3_t &dir, const vec3_t &a, const vec3_t &b, const vec3_t &c);

vec3_t closest_point_on_triangle(const vec3_t &point, const vec3_t &a, const vec3_t &b, const vec3_t &c);

void closest_point_line_triangle(const vec3_t &v0, const vec3_t &v1, const vec3_t &v2, const vec3_t &a, const vec3_t &b, vec3_t *point_on_tri, vec3_t *point_on_line);


#endif // GEOMETRY_H
