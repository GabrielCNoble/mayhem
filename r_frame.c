#include "r_frame.h"
#include "matrix.h"

extern struct renderer_t r_renderer;

void r_DrawFrame()
{
    mat4_t proj;

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //r_Perspective(0.5, 320.0 / 240.0, 0.1, 500.0, &proj, NULL);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float *)r_renderer.active_view->projection_matrix.floats);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float *)r_renderer.active_view->view_matrix.floats);

    glBegin(GL_TRIANGLES);

    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-0.2, -0.1, -5.5);
    glVertex3f(0.2, -0.1, -5.5);
    glVertex3f(0.0, 0.2, -5.5);
    glEnd();

    SDL_GL_SwapWindow(r_renderer.window);
}
