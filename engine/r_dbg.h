#ifndef R_DBG_H
#define R_DBG_H

#include "../common/gmath/vector.h"
#include "../common/gmath/matrix.h"

struct draw_line_data_t
{
    vec3_t a;
    vec3_t b;
    vec3_t color;
};

struct draw_triangle_data_t
{
    vec3_t a;
    vec3_t b;
    vec3_t c;
    vec3_t color;

    unsigned wireframe : 1;
    unsigned backface_culling : 1;
    unsigned depth_test : 1;
};

struct draw_capsule_data_t
{
    vec3_t position;
    float radius;
    float height;
};

struct draw_point_data_t
{
    vec3_t position;
    vec3_t color;
    float size;
    int smooth;
};

struct draw_debug_data_t
{
    mat4_t view_projection_matrix;
};


#ifdef __cplusplus
extern "C"
{
#endif

void r_InitDebug();

void r_ShutdownDebug();

void r_DrawLine(vec3_t a, vec3_t b, vec3_t color);

void r_i_DrawLine(struct draw_line_data_t *line);

void r_DrawTriangle(vec3_t a, vec3_t b, vec3_t c, vec3_t color, int wireframe, int backface_culling, int depth_test);

void r_i_DrawTriangle(struct draw_triangle_data_t *triangle);

void r_DrawCapsule(vec3_t position, float height, float radius);

void r_i_DrawCapsule(struct draw_capsule_data_t *capsule);

void r_DrawPoint(vec3_t position, vec3_t color, float size, int smooth);

void r_i_DrawPoint(struct draw_point_data_t *point);

void r_DrawDebug(struct view_t *view);

void r_i_DrawDebug(struct draw_debug_data_t *data);

#ifdef __cplusplus
}
#endif

#endif // R_DBG_H
