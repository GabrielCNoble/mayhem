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
    unsigned short material;
};

struct draw_command_t
{
    mat4_t *transform;
};

struct draw_command_buffer_t
{
    struct list_t draw_commands;
    struct view_t *view;
};

struct view_t
{
    struct view_t *next;

    mat4_t projection_matrix;
    mat4_t view_matrix;

    mat3_t orientation;
    vec3_t position;
    struct frustum_t frustum;

    int draw_command_buffer;
};


struct renderer_t
{
    SDL_Window *window;
    SDL_GLContext context;

    struct view_t *views;
    struct view_t *active_view;

    int window_width;
    int window_height;

    int renderer_width;
    int renderer_height;
};




#endif // R_COMMON_H
