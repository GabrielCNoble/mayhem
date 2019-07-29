#ifndef BE_R_DBG
#define BE_R_DBG

#include "r_dbg.h"



/* implementation of the functions in r_dbg,
meant to be run *only* from the backend... */


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

void be_r_DrawLine(struct draw_line_data_t *line);

void be_r_DrawTriangle(struct draw_triangle_data_t *triangle);

void be_r_DrawCapsule(struct draw_capsule_data_t *capsule);

void be_r_DrawPoint(struct draw_point_data_t *point);

void be_r_DrawDebug(struct draw_debug_data_t *data);


#endif // BE_R_DBG
