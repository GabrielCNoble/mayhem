#ifndef R_VIEW_H
#define R_VIEW_H


#include "r_common.h"
#include "matrix.h"


void r_Perspective(float fov_y, float aspect, float z_near, float z_far, mat4_t *mat, struct frustum_t *frustum);

void r_UpdateActiveView();

void r_ComputeViewMatrix(struct view_t *view);

struct view_t *r_CreateView();

void r_SetView(struct view_t *view);

struct view_t *r_GetActiveView();






#endif // R_VIEW_H
