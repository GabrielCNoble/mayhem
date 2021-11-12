#ifndef R_COMMON_H
#define R_COMMON_H


#include "libs/SDL/include/SDL2/SDL.h"
#include "libs/GLEW/include/GL/glew.h"

#include "../common/gmath/matrix.h"
#include "../common/containers/list.h"
#include "../common/containers/ring_buffer.h"

#define R_MAX_WIDTH 1920
#define R_MAX_HEIGHT 1080

#define R_MIN_WIDTH 1300
#define R_MIN_HEIGHT 760


#define R_MATRIX_STACK_DEPTH 255

#define R_MAX_RAYTRACE_TRIANGLES 8192
#define R_MAX_LIGHTS 64

#define R_LIGHTS_UNIFORM_BUFFER_BINDING 0
#define R_TRIANGLES_UNIFORM_BUFFER_BINDING 1
#define R_TRIANGLE_INDICES_UNIFORM_BUFFER_BINDING 2


/*
========================================
========================================
========================================
*/
struct light_t
{
    vec4_t pos_radius;
    vec4_t col_energy;
    vec4_t w_pos;
};

struct light_interface_t
{
    struct light_t lights[R_MAX_LIGHTS];
    int light_count;
};

struct material_interface_t
{
    vec4_t base_color;
    int material_flags;
};


struct raytrace_triangle_t
{
    vec4_t vertices[3];
};

struct triangle_interface_t
{
    struct raytrace_triangle_t triangles[R_MAX_RAYTRACE_TRIANGLES];
};




struct triangle_indice_interface_t
{
    unsigned int indices[R_MAX_RAYTRACE_TRIANGLES];
};

/*
========================================
========================================
========================================
*/

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
    mat4_t model_view_projection_matrix;
    struct draw_batch_t batch;
    int *indices;
};

struct draw_command_buffer_t
{
    struct list_t draw_commands;
    struct list_t indices;
    mat4_t view_matrix;
    vec4_t near_plane;
    int used;
};

struct light_buffer_t
{
    struct list_t lights;
    int used;
};





enum R_VIEW_TYPE
{
    R_VIEW_TYPE_BASE = 0,
    R_VIEW_TYPE_PLAYER,
};

struct view_t
{
    struct view_t *next;
    int type;

    mat4_t view_projection_matrix;
    mat4_t projection_matrix;
    mat4_t view_matrix;

    vec4_t near_plane;

    mat3_t orientation;
    vec3_t position;
    struct frustum_t frustum;
};



struct uniform_buffer_t
{
    unsigned int size;
    unsigned int offset;
};

#define R_FRAMEBUFFER_MAX_COLOR_ATTACHMENTS 4

struct framebuffer_t
{
    unsigned int framebuffer;

    struct
    {
        unsigned int texture;
        unsigned int format;
        unsigned int internal_format;
        unsigned int type;
    }color_attachments[R_FRAMEBUFFER_MAX_COLOR_ATTACHMENTS];

    unsigned int depth_attachment;
    unsigned int width;
    unsigned int height;
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

    struct framebuffer_t backbuffer;

//    unsigned int backbuffer_framebuffer;
//    unsigned int backbuffer_color_texture;
//    unsigned int backbuffer_depth_texture;

    int model_view_projection_top;
    mat4_t *model_view_projection_stack;


    int lit_shader;
    int portal_stencil_shader;
    int clear_portal_depth_shader;
    int debug_shader;

    int current_stencil;

    unsigned int default_vao;

    int max_recursion;

    unsigned int raytrace_framebuffer;
    unsigned int raytrace_colorbuffer;
    unsigned int raytrace_depthbuffer;

    unsigned int uniform_buffer;
    struct uniform_buffer_t uniform_buffers[3];

    mat4_t projection_matrix;
    mat4_t view_matrix;
    mat4_t view_projection_matrix;

    SDL_SpinLock stream_spinlock;
    struct ring_buffer_t cmd_stream;
    unsigned char *cmd_data_buffer;
};



#define R_UNIFORM_BUFFER_SIZE (sizeof(struct light_interface_t) + sizeof(raytrace_triangle_interface_t))


#endif // R_COMMON_H
