#include "r_portal.h"
#include "../common/containers/stack_list.h"
#include "r_verts.h"
#include "be_backend.h"

#include <stdio.h>



struct stack_list_t r_portals(sizeof(struct portal_t ), 32);

int r_CreatePortal(vec3_t position, vec2_t size)
{
    struct portal_t *portal;
    struct portal_collider_t *collider;
    int portal_index;

    struct vertex_t vertices[6];

    portal_index = r_portals.add(NULL);
    portal = (struct portal_t *)r_portals.get(portal_index);

    portal->view = r_CreateView();
    portal->linked = -1;
    portal->size = size;

    size *= 0.5;

    vertices[0].position = vec3_t(-size.x, size.y, 0.0);
    vertices[0].normal = vec3_t(0.0, 0.0, -1.0);

    vertices[1].position = vec3_t(size.x, size.y, 0.0);
    vertices[1].normal = vec3_t(0.0, 0.0, -1.0);

    vertices[2].position = vec3_t(size.x, -size.y, 0.0);
    vertices[2].normal = vec3_t(0.0, 0.0, -1.0);

    vertices[3].position = vec3_t(size.x, -size.y, 0.0);
    vertices[3].normal = vec3_t(0.0, 0.0, -1.0);

    vertices[4].position = vec3_t(-size.x, -size.y, 0.0);
    vertices[4].normal = vec3_t(0.0, 0.0, -1.0);

    vertices[5].position = vec3_t(-size.x, size.y, 0.0);
    vertices[5].normal = vec3_t(0.0, 0.0, -1.0);

    portal->view->position = position;
    portal->view->orientation.identity();

    portal->handle = r_AllocVerts(sizeof(struct vertex_t) * 6, sizeof(vertex_t));
    portal->position = position;
    portal->orientation.identity();

    portal->collider = phy_CreateCollider(PHY_COLLIDER_TYPE_PORTAL);
    collider = phy_GetPortalColliderPointer(portal->collider);
    collider->base.position = portal->position;

    collider->portal_handle = portal_index;

    be_MemcpyTo(portal->handle, &vertices, sizeof(struct vertex_t) * 6);

    return portal_index;
}

void r_DestroyPortal(int portal_index)
{
    struct portal_t *portal;

    portal = r_GetPortalPointer(portal_index);

    if(portal)
    {
        if(portal->view)
        {
            r_DestroyView(portal->view);
            portal->view = NULL;

            portal = r_GetPortalPointer(portal->linked);

            if(portal)
            {
                portal->linked = -1;
            }
        }
    }
}

void r_LinkPortals(int portal0, int portal1)
{
    struct portal_t *p0;
    struct portal_t *p1;

    p0 = r_GetPortalPointer(portal0);
    p1 = r_GetPortalPointer(portal1);

    if(p0 && p1)
    {
        p0->linked = portal1;
        p1->linked = portal0;
    }
}

struct portal_t *r_GetPortalPointer(int portal_index)
{
    struct portal_t *portal = NULL;

    portal = (struct portal_t *)r_portals.get(portal_index);

    if(portal)
    {
        if(!portal->view)
        {
            portal = NULL;
        }
    }

    return portal;
}

void r_ComputePortalView(int portal_index, struct view_t *view)
{
    struct portal_t *portal;
    struct portal_t *linked;
    mat3_t inverse_orientation;
    vec3_t portal_view_pos;
    vec3_t portal_view_vec;
    vec4_t near_normal;
    vec4_t portal_vec;

    float t;

    portal = r_GetPortalPointer(portal_index);

    if(portal && portal->linked != -1)
    {
        linked = r_GetPortalPointer(portal->linked);

        portal_view_vec = portal->position - view->position;

        inverse_orientation = portal->orientation;
        inverse_orientation.transpose();

        portal_view_pos.x = -dot(portal->orientation[0], portal_view_vec);
        portal_view_pos.y = dot(portal->orientation[1], portal_view_vec);
        portal_view_pos.z = -dot(portal->orientation[2], portal_view_vec);
        portal->view->position = -portal_view_pos[0] * linked->orientation[0] +
                                 -portal_view_pos[1] * linked->orientation[1] +
                                 -portal_view_pos[2] * linked->orientation[2] + linked->position;

        portal->view->orientation = view->orientation * inverse_orientation *
                                    linked->orientation * rotate(linked->orientation[1], 1.0);

        r_UpdateView(portal->view);

        portal_vec = vec4_t(linked->position, 1.0) * portal->view->view_matrix;
        portal_vec.w = 0.0;

        near_normal = vec4_t(-linked->orientation[2], 0.0) * portal->view->view_matrix;
        portal->view->near_plane = vec4_t(near_normal.x, near_normal.y, near_normal.z, -dot(near_normal, portal_vec));
    }
}






