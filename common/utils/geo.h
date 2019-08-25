#ifndef GEO_H
#define GEO_H


#include "../gmath/vector.h"

void geo_ComputeTangents(vec3_t *vertices, vec2_t *tex_coords, vec3_t *tangents, int vertice_count);

vec3_t geo_SupportVertexUprightCapsule(vec3_t dir, float height, float radius);


#endif // GEO_H
