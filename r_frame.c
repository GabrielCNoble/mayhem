#include "r_frame.h"
#include "matrix.h"
#include "physics.h"

#include <stdlib.h>

extern struct renderer_t r_renderer;

vec3_t *verts = NULL;

extern struct bvh_node_t *phy_collision_bvh;

void r_DrawBvh(struct bvh_node_t *node)
{
    static int level = -1;

    int i;

    if(node)
    {
        level++;

        if(node->triangle)
        {
            glBegin(GL_TRIANGLES);
            glColor3f(0.0, 1.0, 0.0);
            for(i = 0; i < 3; i++)
            {
                glVertex3f(node->triangle->verts[i].x, node->triangle->verts[i].y, node->triangle->verts[i].z);
            }
            glEnd();
        }

        glBegin(GL_QUADS);
        glColor3f(1.0, 0.0, 0.0);

        glVertex3f(node->min.x, node->max.y, node->min.z);
        glVertex3f(node->min.x, node->max.y, node->max.z);
        glVertex3f(node->min.x, node->min.y, node->max.z);
        glVertex3f(node->min.x, node->min.y, node->min.z);


        glVertex3f(node->max.x, node->max.y, node->min.z);
        glVertex3f(node->max.x, node->max.y, node->max.z);
        glVertex3f(node->max.x, node->min.y, node->max.z);
        glVertex3f(node->max.x, node->min.y, node->min.z);


        glVertex3f(node->min.x, node->max.y, node->min.z);
        glVertex3f(node->min.x, node->min.y, node->min.z);
        glVertex3f(node->max.x, node->min.y, node->min.z);
        glVertex3f(node->max.x, node->max.y, node->min.z);


        glVertex3f(node->min.x, node->max.y, node->max.z);
        glVertex3f(node->min.x, node->min.y, node->max.z);
        glVertex3f(node->max.x, node->min.y, node->max.z);
        glVertex3f(node->max.x, node->max.y, node->max.z);

        glEnd();

        r_DrawBvh(node->left);
        r_DrawBvh(node->right);

        level--;
    }
}

void r_DrawFrame()
{
    mat4_t proj;

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float *)r_renderer.active_view->projection_matrix.floats);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float *)r_renderer.active_view->view_matrix.floats);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);

    r_DrawBvh(phy_collision_bvh);


    SDL_GL_SwapWindow(r_renderer.window);
}
