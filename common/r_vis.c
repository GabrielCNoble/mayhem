#include "r_vis.h"
#include "r_view.h"
#include "r_portal.h"
#include "r_light.h"
#include "r_dbg.h"
#include "../containers/list.h"
#include "be_backend.h"
#include "../containers/stack_list.h"
#include "ent.h"

#include <stdio.h>


#define R_MAX_COMMAND_BUFFERS 8192
#define R_MAX_LIGHT_BUFFERS 8192
extern struct list_t w_world_batches;
struct stack_list_t r_command_buffers(sizeof(struct draw_command_buffer_t), R_MAX_COMMAND_BUFFERS);
struct stack_list_t r_light_buffers(sizeof(struct light_buffer_t), R_MAX_LIGHT_BUFFERS);

extern struct stack_list_t r_portals;
extern struct stack_list_t r_lights;
extern struct verts_handle_t w_world_verts;
extern struct renderer_t r_renderer;

void r_VisibleWorld()
{
    r_command_buffers.cursor = 0;
    r_command_buffers.free_top = -1;

    r_light_buffers.cursor = 0;
    r_light_buffers.free_top = -1;

    r_VisibleWorldOnView(r_GetActiveView());
}

void r_VisibleWorldOnView(struct view_t *view)
{
    static int current_recursion = -1;
    struct draw_command_buffer_t *cmd_buffer;
    struct light_buffer_t *light_buffer;
    struct draw_command_t cmd;
    int i;
    int j;

    vec4_t view_space_pos;
    mat4_t view_projection_matrix;

    struct view_t cur_view;


    struct portal_t *portals;
    struct light_t *lights;

    portals = (struct portal_t *)r_portals.buffer;

    current_recursion++;

    r_UpdateView(view);

    light_buffer = r_AllocLightBuffer();
    r_VisibleLightsOnView(view, light_buffer);
    be_UploadLights(light_buffer);

    cmd_buffer = r_AllocCommandBuffer();
    cmd_buffer->view_matrix = view->view_matrix;
    cmd_buffer->near_plane = view->near_plane;
    r_VisibleWorldTrianglesOnView(view, cmd_buffer);
    be_DrawLit(cmd_buffer);

    cmd_buffer = r_AllocCommandBuffer();
    cmd_buffer->view_matrix = view->view_matrix;
    cmd_buffer->near_plane = view->near_plane;
    r_VisibleEntitiesOnView(view, cmd_buffer);
    be_DrawLit(cmd_buffer);


//    r_DrawLine(vec3_t(0.0, 5.0, 0.0), vec3_t(0.0, -5.0, 0.0), vec3_t(1.0, 1.0, 1.0));
//
//    r_DrawTriangle(vec3_t(1.0, 0.0, 0.0),
//                   vec3_t(-1.0, 0.0, 0.0),
//                   vec3_t(0.0, 1.0, 0.0),
//                   vec3_t(0.0, 1.0, 0.0),
//                   1, 0, 0);

    cur_view = *view;
//
//    cmd_buffer = r_AllocCommandBuffer();
//    cmd_buffer->view_matrix = view->view_matrix * view->projection_matrix;
//    be_DrawDebug(cmd_buffer);

    r_DrawDebug(view);

    if(current_recursion < r_renderer.max_recursion)
    {
        for(i = 0; i < r_portals.cursor; i++)
        {
            if(portals[i].view && portals[i].linked != -1)
            {
                if(!r_IsPortalVisible(i, &cur_view))
                {
                    continue;
                }

                cmd_buffer = r_AllocCommandBuffer();
                cmd.batch.start = r_GetAllocStart(portals[i].handle) / sizeof(struct vertex_t);
                cmd.model_view_projection_matrix = mat4_t(portals[i].orientation, portals[i].position) * cur_view.view_projection_matrix;
                cmd_buffer->draw_commands.add(&cmd);
                cmd_buffer->view_matrix = mat4_t(portals[i].orientation, portals[i].position) * cur_view.view_matrix;
                cmd_buffer->near_plane = cur_view.near_plane;

                be_AddPortalStencil(cmd_buffer);
                r_ComputePortalView(i, &cur_view);
                r_VisibleWorldOnView(portals[i].view);
                be_RemPortalStencil(cmd_buffer);
            }
        }
    }

    current_recursion--;
}

void r_VisibleEntitiesOnView(struct view_t *view, struct draw_command_buffer_t *cmd_buffer)
{
    struct entity_t *entities;
    struct entity_t *entity;
    struct draw_batch_t *batches;
    struct model_component_t *model_component;
    struct model_t *model;
    struct transform_component_t *transform_component;
//    mat4_t entity_transform;
    mat4_t model_view_projection_matrix;
    struct draw_command_t draw_command;
    int i;
    int c;
    int j;

//    struct view_t *view = r_GetActiveView();


    ent_GetEntityListPointer(&entities, &c, 0);

    for(i = 0; i < c; i++)
    {
        entity = entities + i;

        if(entity->flags & ENT_ENTITY_FLAG_INVALID)
        {
            continue;
        }

        if(entity->components[ENT_COMPONENT_TYPE_MODEL].index == ENT_INVALID_COMPONENT_INDEX)
        {
            continue;
        }

        model_component = (struct model_component_t *)ent_GetComponentPointer(entity->components[ENT_COMPONENT_TYPE_MODEL]);
        transform_component = (struct transform_component_t *)ent_GetComponentPointer(entity->components[ENT_COMPONENT_TYPE_TRANSFORM]);

        model = mdl_GetModelPointer(model_component->model);
        mat4_t entity_transform(transform_component->orientation, transform_component->position);
        model_view_projection_matrix = entity_transform * view->view_projection_matrix;

        batches = (struct draw_batch_t *)model->batches.buffer;

        for(j = 0; j < model->batches.cursor; j++)
        {
            draw_command.model_view_projection_matrix = model_view_projection_matrix;
            draw_command.batch = batches[j];
            draw_command.indices = NULL;
            cmd_buffer->draw_commands.add(&draw_command);
        }
    }
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
        cmd.model_view_projection_matrix = view->view_projection_matrix;
        cmd_buffer->draw_commands.add(&cmd);
    }
}

void r_VisibleLightsOnView(struct view_t *view, struct light_buffer_t *light_buffer)
{
    vec4_t corners[8];
    vec4_t view_space_pos;
    struct light_t *lights;
    struct light_t *light;
    struct light_t copy;

    lights = (struct light_t *)r_lights.buffer;

    float max_x;
    float min_x;
    float max_y;
    float min_y;
    int positive_z;

    int i;
    int j;

    for(i = 0; i < r_lights.cursor; i++)
    {
        light = r_GetLightPointer(i);

        if(!light)
        {
            continue;
        }

        view_space_pos = vec4_t(light->pos_radius[0], light->pos_radius[1], light->pos_radius[2], 1.0) * view->view_matrix;

        corners[0] = vec4_t(-light->pos_radius[3], light->pos_radius[3],-light->pos_radius[3], 1.0);
        corners[1] = vec4_t(-light->pos_radius[3],-light->pos_radius[3],-light->pos_radius[3], 1.0);
        corners[2] = vec4_t( light->pos_radius[3],-light->pos_radius[3],-light->pos_radius[3], 1.0);
        corners[3] = vec4_t( light->pos_radius[3], light->pos_radius[3],-light->pos_radius[3], 1.0);

        corners[4] = vec4_t(-light->pos_radius[3], light->pos_radius[3], light->pos_radius[3], 1.0);
        corners[5] = vec4_t(-light->pos_radius[3],-light->pos_radius[3], light->pos_radius[3], 1.0);
        corners[6] = vec4_t( light->pos_radius[3],-light->pos_radius[3], light->pos_radius[3], 1.0);
        corners[7] = vec4_t( light->pos_radius[3], light->pos_radius[3], light->pos_radius[3], 1.0);

        max_x = -10.0;
        min_x = 10.0;
        max_y = -10.0;
        min_y = 10.0;
        positive_z = 0;

        for(j = 0; j < 8; j++)
        {
            corners[j][0] += view_space_pos[0];
            corners[j][1] += view_space_pos[1];
            corners[j][2] += view_space_pos[2];

            corners[j] = corners[j] * view->projection_matrix;

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

        if((max_x - min_x) * (max_y - min_y) > 0.0 && positive_z < 8)
        {
            copy.col_energy = light->col_energy;
            copy.pos_radius = view_space_pos;
            copy.pos_radius[3] = light->pos_radius[3];

            light_buffer->lights.add(&copy);
        }
    }
}

void r_TraverseBvh(struct bvh_node_t *node, struct draw_batch_t *batches)
{

}

int r_IsPortalVisible(int portal_handle, struct view_t *view)
{
    mat4_t model_view_projection_matrix;
    mat4_t portal_transform;
    struct portal_t *portal;
    vec2_t size;
    vec4_t corners[4];

    int j;

    float max_x = -10.0;
    float max_y = -10.0;
    float min_x = 10.0;
    float min_y = 10.0;
    int positive_z = 0;

    portal = r_GetPortalPointer(portal_handle);

    if(dot(portal->position - view->position, portal->orientation[2]) < 0.0)
    {
        return 0;
    }

    portal_transform[0] = vec4_t(portal->orientation[0], 0.0);
    portal_transform[1] = vec4_t(portal->orientation[1], 0.0);
    portal_transform[2] = vec4_t(portal->orientation[2], 0.0);
    portal_transform[3] = vec4_t(portal->position, 1.0);

    model_view_projection_matrix = portal_transform * view->view_projection_matrix;

    size = portal->size * 0.5;

    corners[0] = vec4_t(size[0], size[1], 0.0, 1.0);
    corners[1] = vec4_t(-size[0], size[1], 0.0, 1.0);
    corners[2] = vec4_t(size[0], -size[1], 0.0, 1.0);
    corners[3] = vec4_t(-size[0], -size[1], 0.0, 1.0);

    for(j = 0; j < 4; j++)
    {
        corners[j] = corners[j] * model_view_projection_matrix;

        if(corners[j].w < 0.0001)
        {
            corners[j].w = 0.0001;
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

    return (max_x - min_x) * (max_y - min_y) > 0.0 && positive_z < 4;
}

struct draw_command_buffer_t *r_AllocCommandBuffer()
{
    struct draw_command_buffer_t *cmd_buffer;
    int cmd_buffer_index;

    if(r_command_buffers.cursor >= R_MAX_COMMAND_BUFFERS)
    {
        while(r_command_buffers.free_top < 0)
        {
            r_ReclaimCommandBuffers();
        }
    }

    cmd_buffer_index = r_command_buffers.add(NULL);
    cmd_buffer = (struct draw_command_buffer_t *)r_command_buffers.get(cmd_buffer_index);

    if(cmd_buffer->draw_commands.buffer == NULL)
    {
        cmd_buffer->draw_commands.init(sizeof(struct draw_command_t), 32);
    }

    if(cmd_buffer->indices.buffer == NULL)
    {
        cmd_buffer->indices.init(sizeof(unsigned int), 256);
    }

    cmd_buffer->draw_commands.cursor = 0;
    cmd_buffer->used = 0;

    return cmd_buffer;
}

struct light_buffer_t *r_AllocLightBuffer()
{
    struct light_buffer_t *light_buffer;
    int light_buffer_index;

    if(r_light_buffers.cursor >= R_MAX_LIGHT_BUFFERS)
    {
        while(r_light_buffers.free_top < 0)
        {
            r_ReclaimLightBuffers();
        }
    }

    light_buffer_index = r_light_buffers.add(NULL);
    light_buffer = (struct light_buffer_t *)r_light_buffers.get(light_buffer_index);

    if(light_buffer->lights.buffer == NULL)
    {
        light_buffer->lights.init(sizeof(struct light_t), 32);
    }

    light_buffer->lights.cursor = 0;

    return light_buffer;
}

void r_ReclaimCommandBuffers()
{
    int i;
    struct draw_command_buffer_t *cmd_buffers;

    cmd_buffers = (struct draw_command_buffer_t *)r_command_buffers.buffer;

    for(i = 0; i < r_command_buffers.cursor; i++)
    {
        if(cmd_buffers[i].used)
        {
            cmd_buffers[i].used = 0;
            r_command_buffers.remove(i);
        }
    }
}

void r_ReclaimLightBuffers()
{
    int i;
    struct light_buffer_t *light_buffers;

    light_buffers = (struct light_buffer_t *)r_light_buffers.buffer;

    for(i = 0; i < r_light_buffers.cursor; i++)
    {
        if(light_buffers[i].used)
        {
            light_buffers[i].used = 0;
            r_light_buffers.remove(i);
        }
    }
}








