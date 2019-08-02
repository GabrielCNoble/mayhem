#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "vector.h"

vec3_t support_vertex_upright_capsule(const vec3_t &dir, float height, float radius, vec3_t &normal);

vec3_t support_vertex_triangle(const vec3_t &dir, const vec3_t &a, const vec3_t &b, const vec3_t &c);

vec3_t closest_point_on_triangle(const vec3_t &point, const vec3_t &a, const vec3_t &b, const vec3_t &c);

int closest_point_line_triangle(const vec3_t &v0, const vec3_t &v1, const vec3_t &v2, const vec3_t &a, const vec3_t &b, vec3_t *point_on_tri, vec3_t *point_on_line, float *line_t);

int intersect_sphere(const vec3_t &center, float radius, const vec3_t &start, const vec3_t &end, float &t0, float &t1);


#endif // GEOMETRY_H
