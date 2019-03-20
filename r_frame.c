#include "r_frame.h"
#include "matrix.h"
#include "physics.h"

#include <stdlib.h>

extern struct renderer_t r_renderer;

vec3_t *verts = NULL;

extern struct bvh_node_t *collision_bvh;

void r_DrawBvh(struct bvh_node_t *node)
{
    static int level = -1;

    if(node)
    {
        level++;

        glBegin(GL_QUADS);
        glColor3f(1.0, 0.0, 0.0);

        glVertex3f(node->min.x, node->max.y + level * 2, node->min.z);
        glVertex3f(node->min.x, node->max.y + level * 2, node->max.z);
        glVertex3f(node->min.x, node->min.y + level * 2, node->max.z);
        glVertex3f(node->min.x, node->min.y + level * 2, node->min.z);


        glVertex3f(node->max.x, node->max.y + level * 2, node->min.z);
        glVertex3f(node->max.x, node->max.y + level * 2, node->max.z);
        glVertex3f(node->max.x, node->min.y + level * 2, node->max.z);
        glVertex3f(node->max.x, node->min.y + level * 2, node->min.z);


        glVertex3f(node->min.x, node->max.y + level * 2, node->min.z);
        glVertex3f(node->min.x, node->min.y + level * 2, node->min.z);
        glVertex3f(node->max.x, node->min.y + level * 2, node->min.z);
        glVertex3f(node->max.x, node->max.y + level * 2, node->min.z);


        glVertex3f(node->min.x, node->max.y + level * 2, node->max.z);
        glVertex3f(node->min.x, node->min.y + level * 2, node->max.z);
        glVertex3f(node->max.x, node->min.y + level * 2, node->max.z);
        glVertex3f(node->max.x, node->max.y + level * 2, node->max.z);

        glEnd();

        r_DrawBvh(node->left);
        r_DrawBvh(node->right);

        level--;
    }
}

void r_DrawFrame()
{
//    int x;
//    int y;
//    int v;
//    int x_count = 2;
//    int y_count = 2;
//
//    vec3_t tris[6] = {vec3_t(-1.0, 0.0, 1.0), vec3_t(1.0, 0.0, 1.0), vec3_t(1.0, 0.0, -1.0),
//                      vec3_t(1.0, 0.0, -1.0), vec3_t(-1.0, 0.0, -1.0), vec3_t(-1.0, 0.0, 1.0)};
//
//
//    if(!verts)
//    {
//        verts = (vec3_t *)calloc(sizeof(tris), x_count + y_count);
//
//        for(y = 0; y < y_count; y++)
//        {
//            for(x = 0; x < x_count; x++)
//            {
//                for(v = 0; v < 6; v++)
//                {
//                    verts[y * x_count * 6 + x * 6 + v] = tris[v] + vec3_t(-x_count / 2 + x * 4, 0.0, -y_count / 2 + y * 4);
//                }
//            }
//        }
//    }

    mat4_t proj;

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //r_Perspective(0.5, 320.0 / 240.0, 0.1, 500.0, &proj, NULL);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float *)r_renderer.active_view->projection_matrix.floats);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float *)r_renderer.active_view->view_matrix.floats);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);

    r_DrawBvh(collision_bvh);
//    glBegin(GL_TRIANGLES);
//
//    glColor3f(1.0, 0.0, 0.0);
//    glVertex3f(-0.2, -0.1, -5.5);
//    glVertex3f(0.2, -0.1, -5.5);
//    glVertex3f(0.0, 0.2, -5.5);
////    for(x = 0; x < 24; x++)
////    {
////        glVertex3f(verts[x].x, verts[x].y, verts[x].z);
////    }
//
//    glEnd();

    SDL_GL_SwapWindow(r_renderer.window);
}
