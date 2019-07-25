#include "be_backend.h"
#include "be_r_dbg.h"
#include "r_common.h"
#include "r_frame.h"
#include "r_shader.h"
#include "r_light.h"
#include "r_surf.h"
#include "input.h"
#include "SDL2/SDL_atomic.h"
#include "../containers/ring_buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdarg.h>




#define BE_CMD_STREAM_BUFFER_SIZE 4096
#define BE_CMD_DATA_BUFFER_SIZE 128

struct
{
    int next_in = 0;
    int next_out = 0;
    SDL_SpinLock stream_spinlock;

    struct backend_cmd_t *cmd_stream;
    unsigned char *cmd_data_buffer;

    void *last_result;
}backend;

extern struct renderer_t r_renderer;

struct ring_buffer_t cmd_stream;

struct memcpy_to_data_t
{
    struct verts_handle_t dst;
    void *src;
    unsigned int size;
};

void be_Init()
{
    backend.cmd_stream = (struct backend_cmd_t *)calloc(sizeof(struct backend_cmd_t), BE_CMD_STREAM_BUFFER_SIZE);
    backend.cmd_data_buffer = (unsigned char *)calloc(BE_CMD_DATA_BUFFER_SIZE, BE_CMD_STREAM_BUFFER_SIZE);
    SDL_AtomicUnlock(&backend.stream_spinlock);

    cmd_stream.init(sizeof(struct backend_cmd_t), BE_CMD_STREAM_BUFFER_SIZE);
}

void be_Shutdown()
{
    free(backend.cmd_stream);
    free(backend.cmd_data_buffer);
}

extern unsigned int r_buffers[2];

void be_RunBackend()
{
    struct backend_cmd_t cmd;
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
        if(cmd_stream.next_out != cmd_stream.next_in)
        {
            be_NextCmd(&cmd, (unsigned char **)&cmd_data);

            switch(cmd.type)
            {
                case BE_CMD_SHUTDOWN:
                    return;
                break;

                case BE_CMD_UPDATE_INPUT:
                    in_UpdateInput();
                break;

                case BE_CMD_SET_SHADER:
                    r_SetShader(*(int *)cmd_data);
                break;

                case BE_CMD_LOAD_SHADER:
                    r_LoadShader(*(char **)cmd_data);
                break;

                case BE_CMD_LOAD_TEXTURE:
//                    loader_thread = SDL_CreateThread(r_LoadTexturePixels, "loader_thread", cmd_data);
//                    SDL_DetachThread(loader_thread);
                break;

                case BE_CMD_UPLOAD_TEXTURE:
                    r_UploadTexturePixels((struct upload_texture_pixels_params_t *)cmd_data);
                break;

                case BE_CMD_DRAW_DBG:
                    r_DrawDebug(*(struct draw_command_buffer_t **)cmd_data, 0);
                break;

                case BE_CMD_DRAW_LINE:
                    be_r_DrawLine((struct draw_line_data_t *)cmd_data);
                break;

                case BE_CMD_DRAW_TRIANGLE:
                    be_r_DrawTriangle((struct draw_triangle_data_t *)cmd_data);
                break;

                case BE_CMD_DRAW_CAPSULE:
                    be_r_DrawCapsule((struct draw_capsule_data_t *)cmd_data);
                break;

                case BE_CMD_DRAW_POINT:
                    be_r_DrawPoint((struct draw_point_data_t *)cmd_data);
                break;

                case BE_CMD_DRAW_LIT:
                    r_DrawLit(*(struct draw_command_buffer_t **)cmd_data);
                break;

                case BE_CMD_ADD_PORTAL_STENCIL:
                    r_PortalStencil(*(struct draw_command_buffer_t **)cmd_data, 0);
                break;

                case BE_CMD_REM_PORTAL_STENCIL:
                    r_PortalStencil(*(struct draw_command_buffer_t **)cmd_data, 1);
                break;

                case BE_CMD_MEMCPY_TO:
                    memcpy_data = (struct memcpy_to_data_t *)cmd_data;
                    r_MemcpyToUnmapped(memcpy_data->dst, memcpy_data->src, memcpy_data->size);
                break;

                case BE_CMD_UPLOAD_LIGHTS:
                    r_UploadLights(*(struct light_buffer_t **)cmd_data);
                break;

                case BE_CMD_CLEAR_BUFFER:
                    r_Clear(*(int *)cmd_data);
                break;

                case BE_CMD_SWAP_BUFFERS:
                    SDL_GL_SwapWindow(r_renderer.window);
                break;
            }

            be_AdvanceQueue();
//            backend.next_out = (backend.next_out + 1) % BE_CMD_STREAM_BUFFER_SIZE;
        }
    }
}

void be_QueueCmd(int type, void *data, int size)
{
    SDL_AtomicLock(&backend.stream_spinlock);

    struct backend_cmd_t cmd;
    int cmd_index;

    if(cmd_stream.available())
    {
        cmd.type = type;
        cmd_index = cmd_stream.add_next_no_advance(&cmd);

        if(data && size)
        {
            memcpy(backend.cmd_data_buffer + cmd_index * BE_CMD_DATA_BUFFER_SIZE, data, size);
        }

        cmd_stream.advance_next_in();
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

    SDL_AtomicUnlock(&backend.stream_spinlock);
}


void be_NextCmd(struct backend_cmd_t *cmd, unsigned char **cmd_data)
{
    SDL_AtomicLock(&backend.stream_spinlock);
//    *cmd = backend.cmd_stream[backend.next_out];
    *cmd = *(struct backend_cmd_t* )cmd_stream.get_next_no_advance();
    *cmd_data = backend.cmd_data_buffer + cmd_stream.next_out * BE_CMD_DATA_BUFFER_SIZE;
    SDL_AtomicUnlock(&backend.stream_spinlock);
}

void be_AdvanceQueue()
{
//    backend.next_out = (backend.next_out + 1) % BE_CMD_STREAM_BUFFER_SIZE;
    cmd_stream.advance_next_out();
}

void be_Clear(int mask)
{
    be_QueueCmd(BE_CMD_CLEAR_BUFFER, &mask, sizeof(int));
}

void be_SwapBuffers()
{
    be_QueueCmd(BE_CMD_SWAP_BUFFERS, NULL, 0);
}

void be_SetShader(int shader_handle)
{
    be_QueueCmd(BE_CMD_SET_SHADER, &shader_handle, sizeof(int));
}

void be_LoadShader(char *file_name)
{
    be_QueueCmd(BE_CMD_LOAD_SHADER, &file_name, sizeof(char *));
    be_WaitEmptyQueue();
}

struct texture_handle_t be_LoadTexture(char *file_name)
{
    be_QueueCmd(BE_CMD_LOAD_TEXTURE, &file_name, sizeof(char *));
    be_WaitEmptyQueue();
    struct texture_handle_t texture;
    return texture;
}

void be_MemcpyTo(struct verts_handle_t dst, void *src, unsigned int size)
{
    struct memcpy_to_data_t data;

    data.dst = dst;
    data.src = src;
    data.size = size;

    be_QueueCmd(BE_CMD_MEMCPY_TO, &data, sizeof(data));
    be_WaitEmptyQueue();
}

void be_DrawDebug(struct draw_command_buffer_t *cmd_buffer)
{
    be_QueueCmd(BE_CMD_DRAW_DBG, &cmd_buffer, sizeof(cmd_buffer));
}

void be_DrawLit(struct draw_command_buffer_t *cmd_buffer)
{
    be_QueueCmd(BE_CMD_DRAW_LIT, &cmd_buffer, sizeof(cmd_buffer));
}

void be_AddPortalStencil(struct draw_command_buffer_t *cmd_buffer)
{
    be_QueueCmd(BE_CMD_ADD_PORTAL_STENCIL, &cmd_buffer, sizeof(cmd_buffer));
}

void be_RemPortalStencil(struct draw_command_buffer_t *cmd_buffer)
{
    be_QueueCmd(BE_CMD_REM_PORTAL_STENCIL, &cmd_buffer, sizeof(cmd_buffer));
}

void be_UploadLights(struct light_buffer_t *light_buffer)
{
    be_QueueCmd(BE_CMD_UPLOAD_LIGHTS, &light_buffer, sizeof(light_buffer));
}

void be_UploadTriangles(vec3_t *vertices, int count)
{
    struct
    {
        vec3_t *vertices;
        int count;
    }data;

    data.vertices = vertices;
    data.count = count;

    be_QueueCmd(BE_CMD_UPLOAD_TRIANGLES, &data, sizeof(data));
}

void be_UploadTriangleIndices(unsigned int *indices, int count)
{
    struct
    {
        unsigned int *indices;
        int count;
    }data;

    data.indices = indices;
    data.count = count;

    be_QueueCmd(BE_CMD_UPLOAD_TRIANGLE_INDICES, &data, sizeof(data));
}

void be_WaitEmptyQueue()
{
    while(cmd_stream.next_in != cmd_stream.next_out);
}


void *be_WaitLastResult()
{
    be_WaitEmptyQueue();
    return backend.last_result;
}






