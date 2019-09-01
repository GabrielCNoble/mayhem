#ifndef R_RAYTRACE_H
#define R_RAYTRACE_H

#include "../common/gmath/vector.h"

#ifdef __cplusplus
extern "C"
{
#endif

void r_InitRaytrace();

void r_ShutdownRaytrace();

void r_UploadTriangles(vec3_t *vertices, int count);

void r_UploadTriangeIndices(unsigned int *indices, int count);

void r_EnableTriangleWrites();

void r_DisableTriangleWrites();

#ifdef __cplusplus
}
#endif

#endif // R_RAYTRACE_H
