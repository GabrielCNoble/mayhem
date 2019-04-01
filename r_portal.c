#include "r_portal.h"
#include "stack_list.h"
#include "r_verts.h"
#include "be_backend.h"



struct stack_list_t r_portals(sizeof(struct portal_t ), 32);

int r_CreatePortal(vec3_t position, vec2_t size)
{
    struct portal_t *portal;
    int portal_index;

    struct vertex_t vertices[6];

    portal_index = r_portals.add(NULL);
    portal = (struct portal_t *)r_portals.get(portal_index);

    portal->view = r_CreateView();
    portal->linked = -1;

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

    be_MemcpyTo(portal->handle, &vertices, sizeof(struct vertex_t) * 6);
    be_WaitEmptyQueue();

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
    mat3_t inverse_portal_orientation;
    vec3_t portal_view_pos;
    vec3_t portal_view_vec;

    portal = r_GetPortalPointer(portal_index);

    if(portal && portal->linked != -1)
    {
        linked = r_GetPortalPointer(portal->linked);

        inverse_portal_orientation = portal->orientation;
        inverse_portal_orientation.transpose();

        portal->view->orientation = view->orientation * inverse_portal_orientation * linked->orientation;

        portal_view_vec = view->position - portal->position;

        portal_view_pos.x = dot(portal->orientation[0], portal_view_vec);
        portal_view_pos.y = dot(portal->orientation[1], portal_view_vec);
        portal_view_pos.z = dot(portal->orientation[2], portal_view_vec);

        portal->view->position = portal_view_pos * linked->orientation + linked->position;
    }
}






