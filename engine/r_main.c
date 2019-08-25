#include "r_main.h"
#include "r_surf.h"
#include "r_shader.h"
#include "r_light.h"
#include "r_dbg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct renderer_t r_renderer;

#ifdef __cplusplus
extern "C"
{
#endif

void r_Init()
{
    int stencil_bits;
    r_renderer.window_width = R_MIN_WIDTH;
    r_renderer.window_height = R_MIN_HEIGHT;
    r_renderer.renderer_width = R_MIN_WIDTH;
    r_renderer.renderer_height = R_MIN_HEIGHT;

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
    def->color[1] = 0xff;
    def->color[2] = 0xff;
    def->color[3] = 0xff;

    r_renderer.max_recursion = 4;

    r_InitVerts();
    r_InitShader();
    r_InitDebug();
    r_InitGui();


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

    r_renderer.backbuffer = r_CreateFramebuffer();
    r_AddFramebufferAttachment(&r_renderer.backbuffer, GL_COLOR_ATTACHMENT0, GL_RGBA, GL_RGBA16F, GL_FLOAT);
    r_AddFramebufferAttachment(&r_renderer.backbuffer, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);

//    glGenTextures(1, &r_renderer.backbuffer_color_texture);
//    glBindTexture(GL_TEXTURE_2D, r_renderer.backbuffer_color_texture);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, R_MIN_WIDTH, R_MIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
//
//    glGenTextures(1, &r_renderer.backbuffer_depth_texture);
//    glBindTexture(GL_TEXTURE_2D, r_renderer.backbuffer_depth_texture);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, R_MIN_WIDTH, R_MIN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//
//    glGenFramebuffers(1, &r_renderer.backbuffer_framebuffer);
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, r_renderer.backbuffer_framebuffer);
//    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, r_renderer.backbuffer_color_texture, 0);
//    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, r_renderer.backbuffer_depth_texture, 0);


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

    if(width > r_renderer.window_width)
    {
        width = r_renderer.window_width;
    }

    if(height > r_renderer.window_height)
    {
        height = r_renderer.window_height;
    }

    if(width != r_renderer.renderer_width || height != r_renderer.renderer_height)
    {
        r_renderer.renderer_width = width;
        r_renderer.renderer_height = height;

        glViewport(0, 0, r_renderer.renderer_width, r_renderer.renderer_height);
        r_SetFramebufferResolution(&r_renderer.backbuffer, r_renderer.renderer_width, r_renderer.renderer_height);

//        glBindTexture(GL_TEXTURE_2D, r_renderer.backbuffer_color_texture);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, r_renderer.renderer_width, r_renderer.renderer_height, 0, GL_RGBA, GL_FLOAT, NULL);
//        glBindTexture(GL_TEXTURE_2D, r_renderer.backbuffer_depth_texture);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, r_renderer.renderer_width, r_renderer.renderer_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

}

struct framebuffer_t r_CreateFramebuffer()
{
    struct framebuffer_t framebuffer;

    memset(&framebuffer, 0, sizeof(struct framebuffer_t ));

    glGenFramebuffers(1, &framebuffer.framebuffer);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.framebuffer);

    framebuffer.width = R_MIN_WIDTH;
    framebuffer.height = R_MIN_HEIGHT;

    return framebuffer;
}

void r_AddFramebufferAttachment(struct framebuffer_t *framebuffer, int attachment, int format, int internal_format, int type)
{
    int min_mag_filter;
    int color_attachment_index;
    unsigned int texture;

    if(framebuffer)
    {
        if(framebuffer->framebuffer)
        {
            switch(attachment)
            {
                case GL_COLOR_ATTACHMENT0:
                case GL_COLOR_ATTACHMENT1:
                case GL_COLOR_ATTACHMENT2:
                case GL_COLOR_ATTACHMENT3:
                    color_attachment_index = attachment - GL_COLOR_ATTACHMENT0;

                    framebuffer->color_attachments[color_attachment_index].format = format;
                    framebuffer->color_attachments[color_attachment_index].internal_format = internal_format;
                    framebuffer->color_attachments[color_attachment_index].type = type;

                    glGenTextures(1, &framebuffer->color_attachments[color_attachment_index].texture);
                    texture = framebuffer->color_attachments[color_attachment_index].texture;
                    min_mag_filter = GL_LINEAR;
                break;

                case GL_DEPTH_ATTACHMENT:
                    glGenTextures(1, &framebuffer->depth_attachment);
                    texture = framebuffer->depth_attachment;
                    min_mag_filter = GL_NEAREST;
                break;

                default:
                    return;
                break;
            }

            glBindTexture(GL_TEXTURE_2D, texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_mag_filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, min_mag_filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format, framebuffer->width, framebuffer->height, 0, format, type, NULL);
            glFramebufferTexture2D(GL_READ_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
        }
    }
}


void r_SetFramebufferResolution(struct framebuffer_t *framebuffer, int width, int height)
{
    int i;

    if(framebuffer)
    {
        if(framebuffer->framebuffer)
        {
            framebuffer->width = width;
            framebuffer->height = height;

            for(i = 0; i < R_FRAMEBUFFER_MAX_COLOR_ATTACHMENTS; i++)
            {
                if(framebuffer->color_attachments[i].texture)
                {
                    glBindTexture(GL_TEXTURE_2D, framebuffer->color_attachments[i].texture);
                    glTexImage2D(GL_TEXTURE_2D, 0, framebuffer->color_attachments[i].internal_format,
                                                   framebuffer->width,
                                                   framebuffer->height, 0,
                                                   framebuffer->color_attachments[i].format,
                                                   framebuffer->color_attachments[i].type, NULL);
                }
            }

            if(framebuffer->depth_attachment)
            {
                glBindTexture(GL_TEXTURE_2D, framebuffer->depth_attachment);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, framebuffer->width, framebuffer->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            }
        }
    }
}

#ifdef __cplusplus
}
#endif






