#include "r_frame.h"
#include "r_verts.h"
#include "matrix.h"
#include "physics.h"

#include <stdlib.h>

extern struct renderer_t r_renderer;

extern int phy_collision_triangle_count;
vec3_t *tris_lut = NULL;

int color_index = 0;

extern struct bvh_node_t *phy_collision_bvh;

void r_DrawBvh(struct bvh_node_t *node)
{
    static int level = -1;

    int i;

    if(!tris_lut)
    {
        tris_lut = (vec3_t *)calloc(sizeof(vec3_t), phy_collision_triangle_count);

        for(i = 0; i < phy_collision_triangle_count; i++)
        {
            tris_lut[i] = vec3_t((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);
        }
    }

    if(node)
    {
        level++;

        if(node->triangle)
        {
            glBegin(GL_TRIANGLES);
            //glColor3f(1.0, 1.0, 0.0);
            glColor3f(tris_lut[color_index].x, tris_lut[color_index].y, tris_lut[color_index].z);
            for(i = 0; i < 3; i++)
            {
                glVertex3f(node->triangle->verts[i].x, node->triangle->verts[i].y, node->triangle->verts[i].z);
            }
            glEnd();

            color_index++;
        }

//        glBegin(GL_QUADS);
//        glColor3f(1.0, 0.0, 0.0);
//
//        glVertex3f(node->min.x, node->max.y, node->min.z);
//        glVertex3f(node->min.x, node->max.y, node->max.z);
//        glVertex3f(node->min.x, node->min.y, node->max.z);
//        glVertex3f(node->min.x, node->min.y, node->min.z);
//
//
//        glVertex3f(node->max.x, node->max.y, node->min.z);
//        glVertex3f(node->max.x, node->max.y, node->max.z);
//        glVertex3f(node->max.x, node->min.y, node->max.z);
//        glVertex3f(node->max.x, node->min.y, node->min.z);
//
//
//        glVertex3f(node->min.x, node->max.y, node->min.z);
//        glVertex3f(node->min.x, node->min.y, node->min.z);
//        glVertex3f(node->max.x, node->min.y, node->min.z);
//        glVertex3f(node->max.x, node->max.y, node->min.z);
//
//
//        glVertex3f(node->min.x, node->max.y, node->max.z);
//        glVertex3f(node->min.x, node->min.y, node->max.z);
//        glVertex3f(node->max.x, node->min.y, node->max.z);
//        glVertex3f(node->max.x, node->max.y, node->max.z);
//
//        glEnd();

        r_DrawBvh(node->left);
        r_DrawBvh(node->right);

        level--;
    }
}

void r_DrawWorld()
{

}

void r_DrawFrame()
{
    mat4_t proj;

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float *)r_renderer.active_view->projection_matrix.floats);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float *)r_renderer.active_view->view_matrix.floats);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);

    color_index = 0;
    r_DrawBvh(phy_collision_bvh);

    SDL_GL_SwapWindow(r_renderer.window);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
