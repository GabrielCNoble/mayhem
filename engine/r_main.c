//#include "r_backend.h"
//#include "be_r_dbg.h"
#include "r_common.h"
#include "r_frame.h"
#include "r_shader.h"
#include "r_light.h"
#include "r_surf.h"
#include "r_gui.h"
#include "input.h"
#include "r_main.h"
#include "r_dbg.h"
#include "SDL2/SDL_atomic.h"
#include "../common/containers/ring_buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdarg.h>




#define BE_CMD_STREAM_BUFFER_SIZE 4096
#define BE_CMD_DATA_BUFFER_SIZE 128

//struct
//{
//    int next_in = 0;
//    int next_out = 0;
//    SDL_SpinLock stream_spinlock;
//
//    struct backend_cmd_t *cmd_stream;
//    unsigned char *cmd_data_buffer;
//
//    void *last_result;
//}backend;

struct renderer_t r_renderer;

//struct ring_buffer_t cmd_stream;

struct memcpy_to_data_t
{
    struct verts_handle_t dst;
    void *src;
    unsigned int size;
};

#ifdef __cplusplus
extern "C"
{
#endif

int32_t r_Init()
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
        return 1;
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

    int uniform_buffer_size = sizeof(struct light_interface_t);

    glGenBuffers(1, &r_renderer.uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, r_renderer.uniform_buffer);
    glBufferData(GL_UNIFORM_BUFFER, uniform_buffer_size, NULL, GL_STREAM_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, r_renderer.uniform_buffer, 0, uniform_buffer_size);

    r_renderer.backbuffer = r_CreateFramebuffer();
    r_AddFramebufferAttachment(&r_renderer.backbuffer, GL_COLOR_ATTACHMENT0, GL_RGBA, GL_RGBA16F, GL_FLOAT);
    r_AddFramebufferAttachment(&r_renderer.backbuffer, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);


//    backend.cmd_stream = (struct backend_cmd_t *)calloc(sizeof(struct backend_cmd_t), BE_CMD_STREAM_BUFFER_SIZE);
    r_renderer.cmd_stream.init(sizeof(struct r_cmd_t), BE_CMD_STREAM_BUFFER_SIZE);
    r_renderer.cmd_data_buffer = (unsigned char *)calloc(BE_CMD_DATA_BUFFER_SIZE, BE_CMD_STREAM_BUFFER_SIZE);
    SDL_AtomicUnlock(&r_renderer.stream_spinlock);

    return 0;
//    cmd_stream.init(sizeof(struct backend_cmd_t), BE_CMD_STREAM_BUFFER_SIZE);
}

void r_Shutdown()
{
//    free(backend.cmd_stream);
//    free(backend.cmd_data_buffer);

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

extern unsigned int r_buffers[2];

void r_Main()
{
    struct r_cmd_t cmd;
    struct memcpy_to_data_t *memcpy_data;
    void *cmd_data;
    int matrix_mode;
    int mask;
    SDL_Thread *loader_thread;

    struct texture_handle_t texture_result;

    glStencilMask(0xff);
    glClearStencil(0);
    glClearDepth(1.0);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CLIP_DISTANCE0);
//    glEnable(GL_CLIP_PLANE0);

    r_renderer.current_stencil = 0;

    r_EnableVertsReads();
//    r_EnableLightWrites();

    while(1)
    {
//        printf("be_RunBackend: start -- ");
        if(r_renderer.cmd_stream.next_out != r_renderer.cmd_stream.next_in)
        {
//            printf("be_RunBackend: execute cmd\n");

            r_NextCmd(&cmd, (unsigned char **)&cmd_data);

            switch(cmd.type)
            {
                case R_CMD_SHUTDOWN:
                    return;
                break;

                case R_CMD_UPDATE_INPUT:
                    in_be_UpdateInput();
                break;

                case R_CMD_SET_SHADER:
                    r_SetShader(*(int *)cmd_data);
                break;

                case R_CMD_LOAD_SHADER:
                    r_LoadShader(*(char **)cmd_data);
                break;

                case R_CMD_LOAD_TEXTURE:
//                    loader_thread = SDL_CreateThread(r_LoadTexturePixels, "loader_thread", cmd_data);
//                    SDL_DetachThread(loader_thread);
                break;

                case R_CMD_UPLOAD_TEXTURE:
                    r_UploadTexturePixels((struct upload_texture_pixels_params_t *)cmd_data);
                break;

                case R_CMD_DRAW_DBG:
                    r_i_DrawDebug((struct draw_debug_data_t *)cmd_data);
                break;

                case R_CMD_DRAW_LINE:
                    r_i_DrawLine((struct draw_line_data_t *)cmd_data);
                break;

                case R_CMD_DRAW_TRIANGLE:
                    r_i_DrawTriangle((struct draw_triangle_data_t *)cmd_data);
                break;

                case R_CMD_DRAW_CAPSULE:
                    r_i_DrawCapsule((struct draw_capsule_data_t *)cmd_data);
                break;

                case R_CMD_DRAW_POINT:
                    r_i_DrawPoint((struct draw_point_data_t *)cmd_data);
                break;

                case R_CMD_DRAW_LIT:
                    r_DrawLit(*(struct draw_command_buffer_t **)cmd_data);
                break;

                case R_CMD_DRAW_GUI:
                    r_be_DrawGui();
                break;

                case R_CMD_ADD_PORTAL_STENCIL:
                    r_PortalStencil(*(struct draw_command_buffer_t **)cmd_data, 0);
                break;

                case R_CMD_REM_PORTAL_STENCIL:
                    r_PortalStencil(*(struct draw_command_buffer_t **)cmd_data, 1);
                break;

                case R_CMD_MEMCPY_TO:
                    memcpy_data = (struct memcpy_to_data_t *)cmd_data;
                    r_MemcpyToUnmapped(memcpy_data->dst, memcpy_data->src, memcpy_data->size);
                break;

                case R_CMD_UPLOAD_LIGHTS:
                    r_UploadLights(*(struct light_buffer_t **)cmd_data);
                break;

                case R_CMD_CLEAR_BUFFER:
                    r_be_Clear(*(int *)cmd_data);
                break;

                case R_CMD_SWAP_BUFFERS:
                    r_i_SwapBuffers();
                break;
            }

            r_AdvanceQueue();
//            backend.next_out = (backend.next_out + 1) % BE_CMD_STREAM_BUFFER_SIZE;
        }
//        else
//        {
//            printf("be_RunBackend: spin\n");
//        }

//        printf("be_RunBackend: end\n");
    }
}

void r_QueueCmd(int type, void *data, int size)
{
    SDL_AtomicLock(&r_renderer.stream_spinlock);

    struct r_cmd_t cmd;
    int cmd_index;

    if(r_renderer.cmd_stream.available())
    {
        cmd.type = type;
        cmd_index = r_renderer.cmd_stream.add_next_no_advance(&cmd);

        if(data && size)
        {
            memcpy(r_renderer.cmd_data_buffer + cmd_index * BE_CMD_DATA_BUFFER_SIZE, data, size);
        }

        r_renderer.cmd_stream.advance_next_in();
    }

//    int in;
//    int out;
//    int diff;
//
//    in = backend.next_in;
//    out = backend.next_out;
//
//    if(in < out)
//    {
//        in += BE_CMD_STREAM_BUFFER_SIZE;
//    }
//
//    diff = BE_CMD_STREAM_BUFFER_SIZE - abs(out - in) - 1;
//
//    if(diff > 0 && backend.cmd_stream)
//    {
//        backend.cmd_stream[backend.next_in].type = type;
//
//        if(data && size)
//        {
//            memcpy(backend.cmd_data_buffer + backend.next_in * BE_CMD_DATA_BUFFER_SIZE, data, size);
//        }
//
//        backend.next_in = (backend.next_in + 1) % BE_CMD_STREAM_BUFFER_SIZE;
//    }

    SDL_AtomicUnlock(&r_renderer.stream_spinlock);
}


void r_NextCmd(struct r_cmd_t *cmd, unsigned char **cmd_data)
{
    SDL_AtomicLock(&r_renderer.stream_spinlock);
//    *cmd = backend.cmd_stream[backend.next_out];
    *cmd = *(struct r_cmd_t* )r_renderer.cmd_stream.get_next_no_advance();
    *cmd_data = r_renderer.cmd_data_buffer + r_renderer.cmd_stream.next_out * BE_CMD_DATA_BUFFER_SIZE;
    SDL_AtomicUnlock(&r_renderer.stream_spinlock);
}

void r_AdvanceQueue()
{
//    backend.next_out = (backend.next_out + 1) % BE_CMD_STREAM_BUFFER_SIZE;
    r_renderer.cmd_stream.advance_next_out();
}

//void be_Clear(int mask)
//{
//    be_QueueCmd(BE_CMD_CLEAR_BUFFER, &mask, sizeof(int));
//}
//
//void be_SwapBuffers()
//{
//    be_QueueCmd(BE_CMD_SWAP_BUFFERS, NULL, 0);
//}

//void be_SetShader(int shader_handle)
//{
//    be_QueueCmd(BE_CMD_SET_SHADER, &shader_handle, sizeof(int));
//}

//void be_LoadShader(char *file_name)
//{
//    be_QueueCmd(BE_CMD_LOAD_SHADER, &file_name, sizeof(char *));
//    be_WaitEmptyQueue();
//}

//struct texture_handle_t be_LoadTexture(char *file_name)
//{
//    be_QueueCmd(BE_CMD_LOAD_TEXTURE, &file_name, sizeof(char *));
//    be_WaitEmptyQueue();
//    struct texture_handle_t texture;
//    return texture;
//}

void be_MemcpyTo(struct verts_handle_t dst, void *src, unsigned int size)
{
    struct memcpy_to_data_t data;

    data.dst = dst;
    data.src = src;
    data.size = size;

    r_QueueCmd(R_CMD_MEMCPY_TO, &data, sizeof(data));
    r_WaitEmptyQueue();
}

void be_DrawDebug(struct draw_command_buffer_t *cmd_buffer)
{
    r_QueueCmd(R_CMD_DRAW_DBG, &cmd_buffer, sizeof(cmd_buffer));
}

void be_DrawLit(struct draw_command_buffer_t *cmd_buffer)
{
    r_QueueCmd(R_CMD_DRAW_LIT, &cmd_buffer, sizeof(cmd_buffer));
}

void be_AddPortalStencil(struct draw_command_buffer_t *cmd_buffer)
{
    r_QueueCmd(R_CMD_ADD_PORTAL_STENCIL, &cmd_buffer, sizeof(cmd_buffer));
}

void be_RemPortalStencil(struct draw_command_buffer_t *cmd_buffer)
{
    r_QueueCmd(R_CMD_REM_PORTAL_STENCIL, &cmd_buffer, sizeof(cmd_buffer));
}

void be_UploadLights(struct light_buffer_t *light_buffer)
{
    r_QueueCmd(R_CMD_UPLOAD_LIGHTS, &light_buffer, sizeof(light_buffer));
}
//
//void be_UploadTriangles(vec3_t *vertices, int count)
//{
//    struct
//    {
//        vec3_t *vertices;
//        int count;
//    }data;
//
//    data.vertices = vertices;
//    data.count = count;
//
//    be_QueueCmd(BE_CMD_UPLOAD_TRIANGLES, &data, sizeof(data));
//}
//
//void be_UploadTriangleIndices(unsigned int *indices, int count)
//{
//    struct
//    {
//        unsigned int *indices;
//        int count;
//    }data;
//
//    data.indices = indices;
//    data.count = count;
//
//    be_QueueCmd(BE_CMD_UPLOAD_TRIANGLE_INDICES, &data, sizeof(data));
//}

void r_WaitEmptyQueue()
{
    while(r_renderer.cmd_stream.next_in != r_renderer.cmd_stream.next_out);
}

//
//void *be_WaitLastResult()
//{
//    be_WaitEmptyQueue();
//    return backend.last_result;
//}

#ifdef __cplusplus
}
#endif





