#include "r_vis.h"
#include "r_view.h"
#include "r_portal.h"
#include "list.h"
#include "be_backend.h"
#include "stack_list.h"

#include <stdio.h>


extern struct list_t w_world_batches;
struct stack_list_t r_command_buffers(sizeof(struct draw_command_buffer_t), 8192);

extern struct stack_list_t r_portals;
extern struct verts_handle_t w_world_verts;

void r_VisibleWorld()
{
    r_ReclaimCommandBuffers();
    r_VisibleWorldOnView(r_GetActiveView());
}

void r_VisibleWorldOnView(struct view_t *view)
{
    static int level = -1;
    struct draw_command_buffer_t *cmd_buffer;
    struct draw_command_t cmd;
    int i;
    int j;

    vec4_t corners[4];

    mat4_t portal_transform;
    mat4_t model_view_projection_matrix;

    struct portal_t *portals;

    float max_x;
    float max_y;
    float min_x;
    float min_y;
    int positive_z;

    float qr;
    float qt;

    level++;

    r_UpdateView(view);

    cmd_buffer = r_AllocCommandBuffer();
    cmd_buffer->view_projection_matrix = view->view_projection_matrix;
    //cmd_buffer->near_plane = view->near_plane;

    qr = -view->frustum.znear / view->frustum.right;
    qt = -view->frustum.znear / view->frustum.top;

    r_VisibleWorldTrianglesOnView(view, cmd_buffer);

    be_DrawLit(cmd_buffer);

    portals = (struct portal_t *)r_portals.buffer;

    if(level < 16)
    {
        for(i = 0; i < r_portals.cursor; i++)
        {
            if(portals[i].view && portals[i].linked != -1)
            {
                if(dot(portals[i].position - view->position, portals[i].orientation[2]) < 0.0)
                {
                    continue;
                }

                portal_transform[0] = vec4_t(portals[i].orientation[0], 0.0);
                portal_transform[1] = vec4_t(portals[i].orientation[1], 0.0);
                portal_transform[2] = vec4_t(portals[i].orientation[2], 0.0);
                portal_transform[3] = vec4_t(portals[i].position, 1.0);

                model_view_projection_matrix = portal_transform * view->view_projection_matrix;

                corners[0] = vec4_t(portals[i].size[0], portals[i].size[1], 0.0, 2.0) * 0.5;
                corners[1] = vec4_t(-portals[i].size[0], portals[i].size[1], 0.0, 2.0) * 0.5;
                corners[2] = vec4_t(portals[i].size[0], -portals[i].size[1], 0.0, 2.0) * 0.5;
                corners[3] = vec4_t(-portals[i].size[0], -portals[i].size[1], 0.0, 2.0) * 0.5;

                max_x = -10.0;
                min_x = 10.0;
                max_y = -10.0;
                min_y = 10.0;

                positive_z = 0;

                for(j = 0; j < 4; j++)
                {
                    corners[j] = corners[j] * model_view_projection_matrix;

                    if(corners[j].w < view->frustum.znear)
                    {
                        corners[j].w = view->frustum.znear;
                        positive_z++;
                    }

                    corners[j].x /= corners[j].w;
                    corners[j].y /= corners[j].w;

                    if(corners[j].x > max_x) max_x = corners[j].x;
                    if(corners[j].x < min_x) min_x = corners[j].x;

                    if(corners[j].y > max_y) max_y = corners[j].y;
                    if(corners[j].y < min_y) min_y = corners[j].y;
                }

                if(max_x > 1.0) max_x = 1.0;
                else if(max_x < -1.0) max_x = -1.0;

                if(min_x > 1.0) min_x = 1.0;
                else if(min_x < -1.0) min_x = -1.0;

                if(max_y > 1.0) max_y = 1.0;
                else if(max_y < -1.0) max_y = -1.0;

                if(min_y > 1.0) min_y = 1.0;
                else if(min_y < -1.0) min_y = -1.0;

                if((max_x - min_x) * (max_y - min_y) <= 0.0 || positive_z >= 4)
                {
                    continue;
                }


                cmd_buffer = r_AllocCommandBuffer();
                cmd.transform[0] = vec4_t(portals[i].orientation[0], 0.0);
                cmd.transform[1] = vec4_t(portals[i].orientation[1], 0.0);
                cmd.transform[2] = vec4_t(portals[i].orientation[2], 0.0);
                cmd.transform[3] = vec4_t(portals[i].position, 1.0);

                cmd_buffer->draw_commands.add(&cmd);
                cmd_buffer->view_projection_matrix = view->view_projection_matrix;
                cmd_buffer->portal_start = r_GetAllocStart(portals[i].handle) / sizeof(struct vertex_t);
                cmd_buffer->remove_stencil = i;

                be_AddPortalStencil(cmd_buffer);
                r_ComputePortalView(i, view);
                r_VisibleWorldOnView(portals[i].view);
                be_RemPortalStencil(cmd_buffer);
            }
        }
    }

    level--;
}

void r_VisibleWorldTrianglesOnView(struct view_t *view, struct draw_command_buffer_t *cmd_buffer)
{
    int i;
    struct draw_batch_t *batches;
    struct draw_command_t cmd;

    batches = (struct draw_batch_t *)w_world_batches.buffer;

    for(i = 0; i < w_world_batches.cursor; i++)
    {
        cmd.batch = batches[i];
        cmd.batch.start += r_GetAllocStart(w_world_verts) / sizeof(struct vertex_t);
        cmd.transform.identity();
        cmd_buffer->draw_commands.add(&cmd);
    }
}

void r_TraverseBvh(struct bvh_node_t *node, struct draw_batch_t *batches)
{

}

struct draw_command_buffer_t *r_AllocCommandBuffer()
{
    struct draw_command_buffer_t *cmd_buffer;
    int cmd_buffer_index;

    cmd_buffer_index = r_command_buffers.add(NULL);
    cmd_buffer = (struct draw_command_buffer_t *)r_command_buffers.get(cmd_buffer_index);

    if(cmd_buffer->draw_commands.buffer == NULL)
    {
        cmd_buffer->draw_commands.init(sizeof(struct draw_command_t), 32);
    }

    cmd_buffer->draw_commands.cursor = 0;

    return cmd_buffer;
}

void r_ReclaimCommandBuffers()
{
    r_command_buffers.cursor = 0;
    r_command_buffers.free_top = -1;
}








