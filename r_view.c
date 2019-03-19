#include "r_view.h"

#include <math.h>
#include <stdlib.h>

extern struct renderer_t r_renderer;

void r_Perspective(float fov_y, float aspect, float znear, float zfar, mat4_t *mat, struct frustum_t *frustum)
{
	struct frustum_t f;

	int i, j;

	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			mat->floats[i][j] = 0;
		}
	}

	f.znear = znear;
	f.zfar = zfar;
	f.top = tan(fov_y)*znear;
	f.bottom = -f.top;
	f.right = f.top*aspect;
	f.left = -f.right;

	mat->floats[0][0] = f.znear / f.right;
	mat->floats[1][1] = f.znear / f.top;
	//mat->floats[2][2] = (-f.zfar)/(f.zfar-f.znear);
	mat->floats[2][2] = (-f.zfar + f.znear) / (f.zfar - f.znear);
	//mat->floats[2][2] = -1.0;


	mat->floats[3][2] = ((-2.0) * f.zfar * f.znear) / (f.zfar - f.znear);
	mat->floats[2][3] = -1.0;
	mat->floats[3][0] = 0.0;
	mat->floats[3][1] = 0.0;

	if(frustum)
	{
		*frustum = f;
	}

	return;
}

void r_UpdateActiveView()
{
    struct view_t *view;

    view = r_GetActiveView();

    r_Perspective(0.5, (float)r_renderer.renderer_width / (float)r_renderer.renderer_height, 0.1, 500.0, &view->projection_matrix, &view->frustum);
    r_ComputeViewMatrix(view);
}

void r_ComputeViewMatrix(struct view_t *view)
{
    mat3_t inverse_rotation;
    vec3_t inverse_translation;

    view->view_matrix.identity();

    inverse_rotation = view->orientation;
    inverse_rotation.transpose();

    inverse_translation = -view->position;

    view->view_matrix.floats[0][0] = inverse_rotation.floats[0][0];
    view->view_matrix.floats[0][1] = inverse_rotation.floats[0][1];
    view->view_matrix.floats[0][2] = inverse_rotation.floats[0][2];

    view->view_matrix.floats[1][0] = inverse_rotation.floats[1][0];
    view->view_matrix.floats[1][1] = inverse_rotation.floats[1][1];
    view->view_matrix.floats[1][2] = inverse_rotation.floats[1][2];

    view->view_matrix.floats[2][0] = inverse_rotation.floats[2][0];
    view->view_matrix.floats[2][1] = inverse_rotation.floats[2][1];
    view->view_matrix.floats[2][2] = inverse_rotation.floats[2][2];

    view->view_matrix.floats[3][0] = inverse_rotation[0][0] * inverse_translation[0] +
                                     inverse_rotation[1][0] * inverse_translation[1] +
                                     inverse_rotation[2][0] * inverse_translation[2];

    view->view_matrix.floats[3][1] = inverse_rotation[0][1] * inverse_translation[0] +
                                     inverse_rotation[1][1] * inverse_translation[1] +
                                     inverse_rotation[2][1] * inverse_translation[2];

    view->view_matrix.floats[3][2] = inverse_rotation[0][2] * inverse_translation[0] +
                                     inverse_rotation[1][2] * inverse_translation[1] +
                                     inverse_rotation[2][2] * inverse_translation[2];
}

struct view_t *r_CreateView()
{
    struct view_t *view;

    view = (struct view_t *)calloc(1, sizeof(struct view_t));

    view->orientation.identity();
    view->position = vec3_t(0.0, 0.0, 0.0);

    //r_Perspective(0.5, 320.0 / 240.0, 0.1, 500.0, &view->projection_matrix, &view->frustum);

    view->next = r_renderer.views;
    r_renderer.views = view;
}

void r_SetView(struct view_t *view)
{
    if(view)
    {
        r_renderer.active_view = view;
    }
}

struct view_t *r_GetActiveView()
{
    return r_renderer.active_view;
}




