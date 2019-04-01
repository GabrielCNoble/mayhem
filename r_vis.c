#include "r_vis.h"
#include "r_view.h"
#include "r_portal.h"
#include "list.h"
#include "be_backend.h"
#include "stack_list.h"

#include <stdio.h>


extern struct list_t w_world_batches;
struct stack_list_t r_command_buffers(sizeof(struct draw_command_buffer_t), 512);

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

    mat4_t portal_transform;

    struct portal_t *portals;

    level++;

    r_UpdateView(view);

    cmd_buffer = r_AllocCommandBuffer();
    cmd_buffer->view_projection_matrix = view->view_projection_matrix;

    r_VisibleWorldTrianglesOnView(view, cmd_buffer);

    be_DrawLit(cmd_buffer);

    portals = (struct portal_t *)r_portals.buffer;

    if(level < 1)
    {
        for(i = 0; i < r_portals.cursor; i++)
        {
            if(portals[i].view && portals[i].linked != -1)
            {
                cmd_buffer = r_AllocCommandBuffer();

                cmd.batch.count = 12;
                cmd.batch.start = r_GetAllocStart(portals[i].handle) / sizeof(struct vertex_t);
                cmd.batch.material = 0;
                cmd.transform[0] = vec4_t(portals[i].orientation[0], 0.0);
                cmd.transform[1] = vec4_t(portals[i].orientation[1], 0.0);
                cmd.transform[2] = vec4_t(portals[i].orientation[2], 0.0);
                cmd.transform[3] = vec4_t(portals[i].position, 1.0);

                cmd_buffer->draw_commands.add(&cmd);
                cmd_buffer->view_projection_matrix = view->view_projection_matrix;




                be_AddPortal(cmd_buffer);
                r_ComputePortalView(i, view);
                r_VisibleWorldOnView(portals[i].view);
                be_RemovePortal(cmd_buffer);
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








