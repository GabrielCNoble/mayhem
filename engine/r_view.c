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
//	mat->floats[2][2] = (-f.zfar + f.znear) / (f.zfar - f.znear);
    mat->floats[2][2] = -1.0;

//    vec3_t near = vec3_t(-1.0, 0.0, 0.0);
//    near = normalize(near);
//
//    mat->floats[0][2] = near[0];
//    mat->floats[1][2] = near[1];
//    mat->floats[2][2] = near[2];
	mat->floats[2][3] = -1.0;


	mat->floats[3][2] = ((-2.0) * f.zfar * f.znear) / (f.zfar - f.znear);
	//mat->floats[3][2] = -0.2;
	mat->floats[3][0] = 0.0;
	mat->floats[3][1] = 0.0;

//	vec3_t near = vec3_t(0.8, 0.0, -1.0);
//	near = normalize(near);
//
//    mat->floats[0][3] = near[0];
//	mat->floats[1][3] = near[1];
//	mat->floats[2][3] = near[2];

	if(frustum)
	{
		*frustum = f;
	}

	return;
}

void r_UpdateView(struct view_t *view)
{
    r_Perspective(0.5, (float)r_renderer.renderer_width / (float)r_renderer.renderer_height, 0.1, 500.0, &view->projection_matrix, &view->frustum);
    r_ComputeViewMatrix(view);
    view->view_projection_matrix = view->view_matrix * view->projection_matrix;
}

void r_UpdateActiveView()
{
    r_UpdateView(r_GetActiveView());
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

    view->near_plane = vec4_t(0.0, 0.0, -1.0, 0.1);

    r_Perspective(0.5, (float)r_renderer.renderer_width / (float)r_renderer.renderer_height, 0.1, 500.0, &view->projection_matrix, &view->frustum);

    view->next = r_renderer.views;
    r_renderer.views = view;

    return view;
}

void r_DestroyView(struct view_t *view)
{
    struct view_t *views;
    struct view_t *prev_view;

    if(view)
    {
        views = r_renderer.views;

        prev_view = NULL;

        while(views)
        {
            if(views == view)
            {
                if(prev_view)
                {
                    prev_view->next = view->next;
                }
                else
                {
                    r_renderer.views = r_renderer.views->next;
                }

                free(view);
                return;
            }

            prev_view = views;
            views = views->next;
        }
    }
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

int r_BoxOnView(struct view_t *view, vec3_t &min, vec3_t &max)
{
    vec4_t corners[8];
    vec3_t extents;

    float qr;
    float qt;

    int i;
    int positive_z = 0;

    float x_max;
    float y_max;
    float x_min;
    float y_min;

    float area;

    extents = vfabs(max - min);

    corners[0] = vec4_t(max, 1.0);
    corners[1] = vec4_t(max - vec3_t(0.0, extents.y, 0.0), 1.0);
    corners[2] = vec4_t(max - vec3_t(extents.x, extents.y, 0.0), 1.0);
    corners[3] = vec4_t(max - vec3_t(extents.x, 0.0, 0.0), 1.0);

    corners[4] = vec4_t(min, 1.0);
    corners[5] = vec4_t(min + vec3_t(0.0, extents.y, 0.0), 1.0);
    corners[6] = vec4_t(min + vec3_t(extents.x, extents.y, 0.0), 1.0);
    corners[7] = vec4_t(min + vec3_t(extents.x, 0.0, 0.0), 1.0);

    qt = -view->frustum.znear / view->frustum.top;
    qr = -view->frustum.znear / view->frustum.right;

    x_max = 10.0;
    x_min = -10.0;
    y_max = 10.0;
    y_min = -10.0;

    for(i = 0; i < 8; i++)
    {
        corners[i] = corners[i] * view->view_matrix;

        if(corners[i].z > -view->frustum.znear)
        {
            corners[i].z = -view->frustum.znear;
            positive_z++;
        }

        corners[i].x = (corners[i].x * qr) / corners[i].z;
        corners[i].y = (corners[i].y * qt) / corners[i].z;

        if(corners[i].x > x_max) x_max = corners[i].x;
        if(corners[i].x < x_min) x_min = corners[i].x;

        if(corners[i].y > y_max) y_max = corners[i].y;
        if(corners[i].y < y_min) y_min = corners[i].y;
    }

    if(x_max > 1.0) x_max = 1.0;
    else if(x_max < -1.0) x_max = -1.0;

    if(x_min > 1.0) x_min = 1.0;
    else if(x_min < -1.0) x_min = -1.0;

    if(y_max > 1.0) y_max = 1.0;
    else if(y_max < -1.0) y_max = -1.0;

    if(y_min > 1.0) y_min = 1.0;
    else if(y_min < -1.0) y_min = -1.0;

    area = (x_max - x_min) * (y_max - y_min);

    return area > 0.0 && positive_z < 8;
}









