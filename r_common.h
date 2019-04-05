#ifndef R_COMMON_H
#define R_COMMON_H


#include "SDL2\SDL.h"
#include "GL\glew.h"

#include "matrix.h"
#include "list.h"


struct frustum_t
{
    float znear;
    float zfar;
    float left;
    float right;
    float top;
    float bottom;
};

struct draw_batch_t
{
    unsigned int start;
    unsigned int count;
    int material;
};

struct draw_triangle_t
{
    unsigned int first_vertice;
    int batch_index;
};

struct draw_command_t
{
    mat4_t transform;
    struct draw_batch_t batch;
};

struct draw_command_buffer_t
{
    struct list_t draw_commands;
    mat4_t view_projection_matrix;
    int remove_stencil;
    int portal_start;
};

struct view_t
{
    struct view_t *next;

    mat4_t view_projection_matrix;
    mat4_t projection_matrix;
    mat4_t view_matrix;

    vec4_t near_plane;

    mat3_t orientation;
    vec3_t position;
    struct frustum_t frustum;
};


struct renderer_t
{
    SDL_Window *window;
    SDL_GLContext context;

    struct view_t *views;
    struct view_t *active_view;

    int active_shader;
    int active_material;

    int window_width;
    int window_height;

    int renderer_width;
    int renderer_height;

    int model_view_projection_top;
    mat4_t *model_view_projection_stack;


    int lit_shader;
    int portal_stencil_shader;
    int clear_portal_depth_shader;

    int current_stencil;


    mat4_t projection_matrix;
    mat4_t view_matrix;
};

#define R_MAX_WIDTH 1920
#define R_MAX_HEIGHT 1080

#define R_MIN_WIDTH 640
#define R_MIN_HEIGHT 480


#define R_MATRIX_STACK_DEPTH 255




#endif // R_COMMON_H
