#include "r_main.h"
#include "r_surf.h"
#include "r_shader.h"
#include "r_light.h"

#include <stdio.h>
#include <stdlib.h>

struct renderer_t r_renderer;

#ifdef __cplusplus
extern "C"
{
#endif

void r_Init()
{
    int stencil_bits;
    r_renderer.window_width = 320;
    r_renderer.window_height = 240;
    r_renderer.renderer_width = 320;
    r_renderer.renderer_height = 240;

    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    r_renderer.window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, r_renderer.window_width, r_renderer.window_height, SDL_WINDOW_OPENGL);
    r_renderer.context = SDL_GL_CreateContext(r_renderer.window);

    SDL_GL_MakeCurrent(r_renderer.window, r_renderer.context);

    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil_bits);

    GLenum init_status;

    init_status = glewInit();

    glEnable(GL_DEPTH_TEST);

    if(init_status != GLEW_NO_ERROR)
    {
        printf("oh shit...\n");
        exit(1);
    }

    SDL_GL_SetSwapInterval(1);

    struct view_t *view;

    view = r_CreateView();
    r_SetView(view);

    r_CreateMaterial(vec4_t(1.0, 1.0, 1.0, 1.0), R_INVALID_TEXTURE_HANDLE, R_INVALID_TEXTURE_HANDLE, "default_material");

    struct material_t *def = r_GetMaterialPointer(0);
    def->color[0] = 0xff;
    def->color[1] = 0;
    def->color[2] = 0xff;
    def->color[3] = 0xff;

    r_renderer.max_recursion = 4;

    r_InitVerts();
    r_InitShader();


    r_renderer.uniform_buffers[R_LIGHTS_UNIFORM_BUFFER_BINDING].offset = 0;
    r_renderer.uniform_buffers[R_LIGHTS_UNIFORM_BUFFER_BINDING].size = sizeof(struct light_interface_t);
//
//    r_renderer.uniform_buffers[R_TRIANGLES_UNIFORM_BUFFER_BINDING].offset = sizeof(struct light_interface_t);
//    r_renderer.uniform_buffers[R_TRIANGLES_UNIFORM_BUFFER_BINDING].size = sizeof(struct triangle_interface_t);
//
//    r_renderer.uniform_buffers[R_TRIANGLE_INDICES_UNIFORM_BUFFER_BINDING].offset = sizeof(struct light_interface_t) +
//                                                                                   sizeof(struct triangle_interface_t);
//    r_renderer.uniform_buffers[R_TRIANGLE_INDICES_UNIFORM_BUFFER_BINDING].size = sizeof(struct triangle_indice_interface_t);


//    int uniform_buffer_size = sizeof(struct light_interface_t) +
//                              sizeof(struct triangle_interface_t) +
//                              sizeof(struct triangle_indice_interface_t);


    int uniform_buffer_size = sizeof(struct light_interface_t);

    glGenBuffers(1, &r_renderer.uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, r_renderer.uniform_buffer);
    glBufferData(GL_UNIFORM_BUFFER, uniform_buffer_size, NULL, GL_STREAM_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, r_renderer.uniform_buffer, 0, uniform_buffer_size);


//    for(int i = 0; i < R_TRIANGLE_INDICES_UNIFORM_BUFFER_BINDING; i++)
//    {
//        int size = r_renderer.uniform_buffers[i].size;
//        int offset = r_renderer.uniform_buffers[i].offset;
//        glBindBufferRange(GL_UNIFORM_BUFFER, i, r_renderer.uniform_buffer, offset, size);
//    }


//
//    glGenFramebuffers(1, &r_renderer.raytrace_framebuffer);
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, r_renderer.raytrace_framebuffer);

//
//    glGenTextures(1, &r_renderer.raytrace_colorbuffer);
//    glBindTexture(GL_TEXTURE_2D, r_renderer.raytrace_colorbuffer);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED)

}

void r_Shutdown()
{
    r_ShutdownVerts();
    SDL_GL_DeleteContext(r_renderer.context);
    SDL_DestroyWindow(r_renderer.window);
}

void r_SetWindowSize(int width, int height)
{
    SDL_Rect display_bounds;
    int display_index;

    if(width && height)
    {
        if(width > R_MAX_WIDTH)
        {
            width = R_MAX_WIDTH;
        }
        else if(width < R_MIN_WIDTH)
        {
            width = R_MIN_WIDTH;
        }

        if(height > R_MAX_HEIGHT)
        {
            height = R_MAX_HEIGHT;
        }
        else if(height < R_MIN_HEIGHT)
        {
            height = R_MIN_HEIGHT;
        }

        display_index = SDL_GetWindowDisplayIndex(r_renderer.window);

        SDL_GetDisplayBounds(display_index, &display_bounds);

        if(width > display_bounds.w)
        {
            width = display_bounds.w;
        }

        if(height > display_bounds.h)
        {
            height = display_bounds.h;
        }

        r_renderer.window_width = width;
        r_renderer.window_height = height;

        SDL_SetWindowSize(r_renderer.window, r_renderer.window_width, r_renderer.window_height);
        SDL_SetWindowPosition(r_renderer.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
}

void r_SetRendererResolution(int width, int height)
{
    if(width && height)
    {
        if(width > R_MAX_WIDTH)
        {
            width = R_MAX_WIDTH;
        }
        else if(width < R_MIN_WIDTH)
        {
            width = R_MIN_WIDTH;
        }

        if(height > R_MAX_HEIGHT)
        {
            height = R_MAX_HEIGHT;
        }
        else if(height < R_MIN_HEIGHT)
        {
            height = R_MIN_HEIGHT;
        }

        r_renderer.renderer_width = width;
        r_renderer.renderer_height = height;

        glViewport(0, 0, r_renderer.renderer_width, r_renderer.renderer_height);
    }
}

#ifdef __cplusplus
}
#endif






