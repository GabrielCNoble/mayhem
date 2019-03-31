#include "r_frame.h"
#include "r_verts.h"
#include "r_shader.h"
#include "r_surf.h"
#include "matrix.h"
#include "physics.h"

#include <stdlib.h>

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

void r_Draw(struct draw_command_buffer_t *cmd_buffer)
{
    int i;
    int c;
    struct draw_command_t *draw_cmds;
    struct view_t *view;

    mat4_t view_projection_matrix;
    mat4_t model_view_projection_matrix;

    draw_cmds = (struct draw_command_t *)cmd_buffer->draw_commands.buffer;
    c = cmd_buffer->draw_commands.cursor;

    view = cmd_buffer->view;

    view_projection_matrix = view->view_matrix * view->projection_matrix;

    for(i = 0; i < c; i++)
    {
        if(draw_cmds[i].transform)
        {
            model_view_projection_matrix = *draw_cmds[i].transform * view_projection_matrix;
        }
        else
        {
            model_view_projection_matrix = view_projection_matrix;
        }

        r_SetMaterial(draw_cmds[i].batch.material);

        r_DefaultUniformMatrix4fv(r_ModelViewProjectionMatrix, (float *)model_view_projection_matrix.floats);

        glDrawArrays(GL_TRIANGLES, draw_cmds[i].batch.start, draw_cmds[i].batch.count);
    }
}

void r_MatrixMode(int mode)
{
//    switch(mode)
//    {
//        case GL_MODELVIEW:
//            r_current_matrix_stack = 0;
//        break;
//
//        case GL_PROJECTION:
//
//        break;
//    }
}





