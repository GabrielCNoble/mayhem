#include "r_frame.h"
#include "r_verts.h"
#include "r_shader.h"
#include "r_surf.h"
#include "../common/gmath/matrix.h"
#include "physics.h"

#include "../common/containers/stack_list.h"

#include <stdlib.h>
#include <stdio.h>

extern struct renderer_t r_renderer;
extern struct stack_list_t phy_colliders[PHY_COLLIDER_TYPE_NONE];
extern struct stack_list_t phy_dbvh_nodes;
extern int phy_dbvh_root;

void r_DrawDbvh(int node_index)
{
    struct dbvh_node_t *node;

    node = phy_GetDbvhNodePointer(node_index);

    vec3_t extents;
    vec3_t center;

    if(node)
    {
        center = (node->max + node->min) / 2.0;
        extents = node->max - center;

        glVertex3f(center[0] - extents[0], center[1] + extents[1], center[2] - extents[2]);
        glVertex3f(center[0] - extents[0], center[1] + extents[1], center[2] + extents[2]);
        glVertex3f(center[0] + extents[0], center[1] + extents[1], center[2] + extents[2]);
        glVertex3f(center[0] + extents[0], center[1] + extents[1], center[2] - extents[2]);


        glVertex3f(center[0] - extents[0], center[1] - extents[1], center[2] - extents[2]);
        glVertex3f(center[0] - extents[0], center[1] - extents[1], center[2] + extents[2]);
        glVertex3f(center[0] + extents[0], center[1] - extents[1], center[2] + extents[2]);
        glVertex3f(center[0] + extents[0], center[1] - extents[1], center[2] - extents[2]);


        glVertex3f(center[0] - extents[0], center[1] + extents[1], center[2] - extents[2]);
        glVertex3f(center[0] - extents[0], center[1] - extents[1], center[2] - extents[2]);
        glVertex3f(center[0] - extents[0], center[1] - extents[1], center[2] + extents[2]);
        glVertex3f(center[0] - extents[0], center[1] + extents[1], center[2] + extents[2]);


        glVertex3f(center[0] + extents[0], center[1] + extents[1], center[2] - extents[2]);
        glVertex3f(center[0] + extents[0], center[1] - extents[1], center[2] - extents[2]);
        glVertex3f(center[0] + extents[0], center[1] - extents[1], center[2] + extents[2]);
        glVertex3f(center[0] + extents[0], center[1] + extents[1], center[2] + extents[2]);

        r_DrawDbvh(node->children[0]);
        r_DrawDbvh(node->children[1]);
    }
}

void r_InitFrame()
{

}

void r_ShutdownFrame()
{

}
//
//void r_DrawDebug(struct draw_command_buffer_t *cmd_buffer, int options)
//{
////    r_SetShader(-1);
//
//    int i;
//    int type;
//    int c;
//    struct player_collider_t *player_colliders;
//    struct player_collider_t *player_collider;
//
//    struct rigid_body_collider_t *rigid_colliders;
//    struct rigid_body_collider_t *rigid_collider;
//    struct base_shape_t *base_shape;
//    struct box_shape_t *box_shape;
//
//    vec3_t pos;
//    vec3_t corner;
//    vec3_t size;
//    float radius;
//    float height;
//
//    r_SetShader(r_renderer.debug_shader);
//    r_DefaultUniformMatrix4fv(r_ViewProjectionMatrix, (float *)active_view->view_projection_matrix.floats);
////    glMatrixMode(GL_PROJECTION);
////    glLoadMatrixf((float *)cmd_buffer->view_matrix.floats);
////    glMatrixMode(GL_MODELVIEW);
////    glLoadIdentity();
//
//    for(type = PHY_COLLIDER_TYPE_PLAYER; type < PHY_COLLIDER_TYPE_NONE; type++)
//    {
//        c = phy_colliders[type].cursor;
//
//        switch(type)
//        {
//            case PHY_COLLIDER_TYPE_PLAYER:
//                #define SEGMENT_VERT_COUNT 64
//                struct vertex_t vertices[SEGMENT_VERT_COUNT * 3];
//
//                vec4_t color = vec4_t(0.0, 1.0, 0.0, 1.0);
//                r_DefaultUniform4fv(r_DebugColor, (float *)color.floats);
//
//                for(i = 0; i < c; i++)
//                {
//                    player_collider = (struct player_collider_t *)phy_colliders[type].get(i);
//
//                    if(player_collider->base.type != PHY_COLLIDER_TYPE_PLAYER)
//                    {
//                        continue;
//                    }
//
//                    r_MemcpyToUnmapped(r_debug_verts_buffer_handle, &vertices, sizeof(struct vertex_t ) * SEGMENT_VERT_COUNT * 3);
//
//                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//                    glDrawArrays(GL_LINE_LOOP, r_debug_verts_buffer_start, SEGMENT_VERT_COUNT);
//                    glDrawArrays(GL_LINE_LOOP, r_debug_verts_buffer_start + SEGMENT_VERT_COUNT, SEGMENT_VERT_COUNT);
//                    glDrawArrays(GL_LINE_LOOP, r_debug_verts_buffer_start + SEGMENT_VERT_COUNT * 2, SEGMENT_VERT_COUNT);
//                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//                }
//
//                #undef SEGMENT_VERT_COUNT
//            break;
//        }
//    }
//
////    player_colliders = (struct player_collider_t *)phy_colliders[PHY_COLLIDER_TYPE_PLAYER].buffer;
//    c = phy_colliders[PHY_COLLIDER_TYPE_PLAYER].cursor;
//
//    //if(options & R_DEBUG_DRAW_OPTION_COLLIDERS)
////    {
////        for(i = 0; i < c; i++)
////        {
////            player_collider = player_colliders + i;
////
////            if(player_collider->base.type == PHY_COLLIDER_TYPE_NONE)
////            {
////                continue;
////            }
////            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
////            glDisable(GL_CULL_FACE);
////            glLineWidth(2.0);
////            glColor3f(1.0, 1.0, 1.0);
////            glBegin(GL_QUADS);
////
////            radius = player_collider->radius;
////            height = player_collider->height * 0.5;
////            pos = player_collider->base.position;
////
////            glVertex3f(pos[0] - radius, pos[1] + height, pos[2] + radius);
////            glVertex3f(pos[0] - radius, pos[1] + height, pos[2] - radius);
////            glVertex3f(pos[0] + radius, pos[1] + height, pos[2] - radius);
////            glVertex3f(pos[0] + radius, pos[1] + height, pos[2] + radius);
////
////
////            glVertex3f(pos[0] - radius, pos[1] - height, pos[2] + radius);
////            glVertex3f(pos[0] - radius, pos[1] - height, pos[2] - radius);
////            glVertex3f(pos[0] + radius, pos[1] - height, pos[2] - radius);
////            glVertex3f(pos[0] + radius, pos[1] - height, pos[2] + radius);
////
////
////            glVertex3f(pos[0] - radius, pos[1] + height, pos[2] + radius);
////            glVertex3f(pos[0] - radius, pos[1] - height, pos[2] + radius);
////            glVertex3f(pos[0] - radius, pos[1] - height, pos[2] - radius);
////            glVertex3f(pos[0] - radius, pos[1] + height, pos[2] - radius);
////
////
////            glVertex3f(pos[0] + radius, pos[1] + height, pos[2] + radius);
////            glVertex3f(pos[0] + radius, pos[1] - height, pos[2] + radius);
////            glVertex3f(pos[0] + radius, pos[1] - height, pos[2] - radius);
////            glVertex3f(pos[0] + radius, pos[1] + height, pos[2] - radius);
////
////
////            glEnd();
////            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
////            glEnable(GL_CULL_FACE);
////            glLineWidth(1.0);
////        }
////    }
//
//
//
//
////    rigid_colliders = (struct rigid_body_collider_t *)phy_colliders[PHY_COLLIDER_TYPE_RIGID].buffer;
////    c = phy_colliders[PHY_COLLIDER_TYPE_RIGID].cursor;
////
////    for(i = 0; i < c; i++)
////    {
////        rigid_collider = rigid_colliders + i;
////
////        if(rigid_collider->base.type == PHY_COLLIDER_TYPE_NONE)
////        {
////            continue;
////        }
////
////        base_shape = phy_GetCollisionShapePointer(rigid_collider->collision_shape);
////
////        glPointSize(8.0);
////        glBegin(GL_POINTS);
////        glColor3f(0.0, 1.0, 0.0);
////
////        switch(base_shape->type)
////        {
////            case PHY_COLLISION_SHAPE_BOX:
////                box_shape = (struct box_shape_t *)base_shape;
////                size = box_shape->size * 0.5;
////
////                corner = size * rigid_collider->orientation + rigid_collider->base.position;
////                glVertex3f(corner[0], corner[1], corner[2]);
////
////                corner = vec3_t(-size[0], size[1], size[2]) *
////                    rigid_collider->orientation + rigid_collider->base.position;
////                glVertex3f(corner[0], corner[1], corner[2]);
////
////                corner = vec3_t(-size[0], size[1], -size[2]) *
////                    rigid_collider->orientation + rigid_collider->base.position;
////                glVertex3f(corner[0], corner[1], corner[2]);
////
////                corner = vec3_t(size[0], size[1], -size[2]) *
////                    rigid_collider->orientation + rigid_collider->base.position;
////                glVertex3f(corner[0], corner[1], corner[2]);
////
////
////
////
////                corner = -size * rigid_collider->orientation + rigid_collider->base.position;
////                glVertex3f(corner[0], corner[1], corner[2]);
////
////                corner = vec3_t(-size[0], -size[1], size[2]) *
////                    rigid_collider->orientation + rigid_collider->base.position;
////                glVertex3f(corner[0], corner[1], corner[2]);
////
////                corner = vec3_t(size[0], -size[1], size[2]) *
////                    rigid_collider->orientation + rigid_collider->base.position;
////                glVertex3f(corner[0], corner[1], corner[2]);
////
////                corner = vec3_t(size[0], -size[1], -size[2]) *
////                    rigid_collider->orientation + rigid_collider->base.position;
////                glVertex3f(corner[0], corner[1], corner[2]);
////            break;
////        }
////
////        glEnd();
////        glPointSize(1.0);
////    }
//
//
//
//
////    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
////    glDisable(GL_CULL_FACE);
////    glColor3f(0.5, 0.2, 1.0);
////    glBegin(GL_QUADS);
////    r_DrawDbvh(phy_dbvh_root);
////    glEnd();
////    glEnable(GL_CULL_FACE);
////    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//    cmd_buffer->used = 1;
//}

void r_DrawLit(struct draw_command_buffer_t *cmd_buffer)
{
    int i;
    int c;
    struct draw_command_t *draw_cmds;
    mat4_t model_view_projection_matrix;

    draw_cmds = (struct draw_command_t *)cmd_buffer->draw_commands.buffer;
    c = cmd_buffer->draw_commands.cursor;

    r_SetShader(r_renderer.lit_shader);
    glStencilFunc(GL_EQUAL, r_renderer.current_stencil, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    r_DefaultUniformMatrix4fv(r_ViewMatrix, (float *)cmd_buffer->view_matrix.floats);
    r_DefaultUniform4fv(r_NearPlane, (float *)cmd_buffer->near_plane.floats);

    for(i = 0; i < c; i++)
    {
        r_SetMaterial(draw_cmds[i].batch.material);
        r_DefaultUniformMatrix4fv(r_ModelViewProjectionMatrix, (float *)draw_cmds[i].model_view_projection_matrix.floats);
        glDrawArrays(GL_TRIANGLES, draw_cmds[i].batch.start, draw_cmds[i].batch.count);
    }

    cmd_buffer->used = 1;
}

void r_PortalStencil(struct draw_command_buffer_t *cmd_buffer, int remove)
{
    struct draw_command_t *cmd;
    mat4_t model_view_projection_matrix;
    int mode;


    cmd = (struct draw_command_t*)cmd_buffer->draw_commands.buffer;

    r_SetShader(r_renderer.portal_stencil_shader);
    r_DefaultUniformMatrix4fv(r_ModelViewProjectionMatrix, (float *) &cmd->model_view_projection_matrix.floats);
    r_DefaultUniformMatrix4fv(r_ViewMatrix, (float *) cmd_buffer->view_matrix.floats);
    r_DefaultUniform4fv(r_NearPlane, (float *) cmd_buffer->near_plane.floats);
    glStencilOp(GL_KEEP, GL_KEEP, remove ? GL_DECR : GL_INCR);
    glStencilFunc(GL_EQUAL, r_renderer.current_stencil, 0xff);
    remove ? r_renderer.current_stencil-- : r_renderer.current_stencil++;

    glEnable(GL_DEPTH_CLAMP);


    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    glDrawArrays(GL_TRIANGLES, cmd->batch.start, 6);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    if(!remove)
    {
        r_SetShader(r_renderer.clear_portal_depth_shader);
        r_DefaultUniformMatrix4fv(r_ModelViewProjectionMatrix, (float *) &cmd->model_view_projection_matrix.floats);
        r_DefaultUniformMatrix4fv(r_ViewMatrix, (float *) cmd_buffer->view_matrix.floats);
        r_DefaultUniform4fv(r_NearPlane, (float *) cmd_buffer->near_plane.floats);

        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilFunc(GL_EQUAL, r_renderer.current_stencil, 0xff);

        glDepthFunc(GL_ALWAYS);
        glDrawArrays(GL_TRIANGLES, cmd->batch.start, 6);
        glDepthFunc(GL_LESS);
    }
    glDisable(GL_DEPTH_CLAMP);

    cmd_buffer->used = 1;
}

void r_DrawRaytracedShadows(struct draw_command_buffer_t *cmd_buffer)
{

}

void r_Clear(int mask)
{
    glClear(mask);
    r_renderer.current_stencil = 0;
}

void r_SwapBuffers()
{
//    while(glGetError() != GL_NO_ERROR);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, r_renderer.backbuffer.framebuffer);
    glBlitFramebuffer(0, 0, r_renderer.renderer_width, r_renderer.renderer_height, 0, 0,
                            r_renderer.window_width, r_renderer.window_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glBlitFramebuffer(0, 0, r_renderer.renderer_width, r_renderer.renderer_height, 0, 0,
                            r_renderer.window_width, r_renderer.window_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);


    SDL_GL_SwapWindow(r_renderer.window);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, r_renderer.backbuffer.framebuffer);
//    glViewport(0, 0, r_renderer.renderer_width, r_renderer.renderer_height);

//    printf("%x\n", glGetError());
}






