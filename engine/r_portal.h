#ifndef R_PORTAL_H
#define R_PORTAL_H

#include "r_view.h"
#include "r_verts.h"
#include "../common/gmath/vector.h"
#include "../common/gmath/matrix.h"
#include "physics.h"

struct portal_t
{
    struct view_t *view;
    mat3_t orientation;
    vec3_t position;
    vec2_t size;

    struct collider_handle_t collider;

    struct verts_handle_t handle;

    int linked;
};

int r_CreatePortal(vec3_t position, vec2_t size);

void r_DestroyPortal(int portal_index);

void r_LinkPortals(int portal0, int portal1);

struct portal_t *r_GetPortalPointer(int portal_index);

void r_ComputePortalView(int portal_index, struct view_t *view);


#endif // R_PORTAL_H
