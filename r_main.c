#include "r_main.h"
#include "r_surf.h"
#include "r_shader.h"

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

    r_InitVerts();
    r_InitShader();
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






