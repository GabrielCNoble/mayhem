#include "r_frame.h"
#include "r_verts.h"
#include "r_shader.h"
#include "r_surf.h"
#include "matrix.h"
#include "physics.h"

#include <stdlib.h>
#include <stdio.h>

extern struct renderer_t r_renderer;

//extern int phy_collision_triangle_count;
//vec3_t *tris_lut = NULL;

//int color_index = 0;

//extern struct bvh_node_t *phy_collision_bvh;



void r_DrawBvh(struct bvh_node_t *node)
{
//    static int level = -1;
//
//    int i;
//
//    if(!tris_lut)
//    {
//        tris_lut = (vec3_t *)calloc(sizeof(vec3_t), phy_collision_triangle_count);
//
//        for(i = 0; i < phy_collision_triangle_count; i++)
//        {
//            tris_lut[i] = vec3_t((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);
//        }
//    }
//
//    if(node)
//    {
//        level++;
//
//        if(node->triangle)
//        {
//            glBegin(GL_TRIANGLES);
//            //glColor3f(1.0, 1.0, 0.0);
//            glColor3f(tris_lut[color_index].x, tris_lut[color_index].y, tris_lut[color_index].z);
//            for(i = 0; i < 3; i++)
//            {
//                glVertex3f(node->triangle->verts[i].x, node->triangle->verts[i].y, node->triangle->verts[i].z);
//            }
//            glEnd();
//
//            color_index++;
//        }
//
////        glBegin(GL_QUADS);
////        glColor3f(1.0, 0.0, 0.0);
////
////        glVertex3f(node->min.x, node->max.y, node->min.z);
////        glVertex3f(node->min.x, node->max.y, node->max.z);
////        glVertex3f(node->min.x, node->min.y, node->max.z);
////        glVertex3f(node->min.x, node->min.y, node->min.z);
////
////
////        glVertex3f(node->max.x, node->max.y, node->min.z);
////        glVertex3f(node->max.x, node->max.y, node->max.z);
////        glVertex3f(node->max.x, node->min.y, node->max.z);
////        glVertex3f(node->max.x, node->min.y, node->min.z);
////
////
////        glVertex3f(node->min.x, node->max.y, node->min.z);
////        glVertex3f(node->min.x, node->min.y, node->min.z);
////        glVertex3f(node->max.x, node->min.y, node->min.z);
////        glVertex3f(node->max.x, node->max.y, node->min.z);
////
////
////        glVertex3f(node->min.x, node->max.y, node->max.z);
////        glVertex3f(node->min.x, node->min.y, node->max.z);
////        glVertex3f(node->max.x, node->min.y, node->max.z);
////        glVertex3f(node->max.x, node->max.y, node->max.z);
////
////        glEnd();
//
//        r_DrawBvh(node->left);
//        r_DrawBvh(node->right);
//
//        level--;
//    }
}

void r_DrawWorld()
{

}

void r_DrawFrame()
{
    mat4_t proj;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);

//    color_index = 0;
 //   r_DrawBvh(phy_collision_bvh);
}

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

    for(i = 0; i < c; i++)
    {
        r_SetMaterial(draw_cmds[i].batch.material);
        r_DefaultUniformMatrix4fv(r_ModelViewProjectionMatrix, (float *)draw_cmds[i].model_view_projection_matrix.floats);
        glDrawArrays(GL_TRIANGLES, draw_cmds[i].batch.start, draw_cmds[i].batch.count);
    }
}

void r_PortalStencil(struct draw_command_buffer_t *cmd_buffer, int remove)
{
    struct draw_command_t *cmd;
    mat4_t model_view_projection_matrix;
    int mode;


    cmd = (struct draw_command_t*)cmd_buffer->draw_commands.buffer;

    r_SetShader(r_renderer.portal_stencil_shader);
    r_DefaultUniformMatrix4fv(r_ModelViewProjectionMatrix, (float *) &cmd->model_view_projection_matrix.floats);
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

        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilFunc(GL_EQUAL, r_renderer.current_stencil, 0xff);

        glDepthFunc(GL_ALWAYS);
        glDrawArrays(GL_TRIANGLES, cmd->batch.start, 6);
        glDepthFunc(GL_LESS);
    }
    glDisable(GL_DEPTH_CLAMP);

}

void r_Clear(int mask)
{
    glClear(mask);
    r_renderer.current_stencil = 0;
}






