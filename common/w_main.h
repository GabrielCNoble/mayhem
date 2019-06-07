#ifndef W_MAIN_H
#define W_MAIN_H

#include "r_common.h"
#include "../gmath/vector.h"
#include "physics.h"

#ifdef __cplusplus
extern "C"
{
#endif


void w_Init();

void w_Shutdown();

void w_LoadLevel(char *file_name);

struct bvh_node_t *w_BuildBvh(struct draw_batch_t *batches, int batch_count, vec3_t *vertices, struct draw_triangle_t *draw_triangles);

#ifdef __cplusplus
}
#endif

#endif // W_MAIN_H
