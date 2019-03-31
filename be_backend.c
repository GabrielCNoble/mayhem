#include "be_backend.h"
#include "r_common.h"
#include "r_frame.h"
#include "r_shader.h"
#include "input.h"
#include "SDL2/SDL_atomic.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdarg.h>




#define BE_CMD_STREAM_BUFFER_SIZE 4096
#define BE_CMD_DATA_BUFFER_SIZE 64

struct
{
    int next_in = 0;
    int next_out = 0;
    SDL_SpinLock stream_spinlock;

    struct backend_cmd_t *cmd_stream;
    unsigned char *cmd_data_buffer;
}backend;

extern struct renderer_t r_renderer;


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

    glBindBuffer(GL_ARRAY_BUFFER, r_buffers[0]);

    while(1)
    {
        if(backend.next_out != backend.next_in)
        {
            SDL_AtomicLock(&backend.stream_spinlock);
            cmd = backend.cmd_stream[backend.next_out];
            cmd_data = backend.cmd_data_buffer + backend.next_out * BE_CMD_DATA_BUFFER_SIZE;
            SDL_AtomicUnlock(&backend.stream_spinlock);

            switch(cmd.type)
            {
                case BE_CMD_SHUTDOWN:
                    return;
                break;

                case BE_CMD_UPDATE_INPUT:
                    in_UpdateInput();
                break;

//                case BE_CMD_TEST:
//                    r_DrawFrame();
//                break;
//
//                case BE_CMD_MATRIX_MODE:
//                    glMatrixMode(*(int *)cmd_data);
//                break;
//
//                case BE_CMD_LOAD_MATRIX:
//                    glLoadMatrixf((const float*) cmd_data);
//                break;

                case BE_CMD_CLEAR_BUFFER:
                    mask = *(int *)cmd_data;
                    glClear(mask);
                break;

                case BE_CMD_SET_SHADER:
                    r_SetShader(*(int *)cmd_data);
                break;

                case BE_CMD_LOAD_SHADER:
                    r_LoadShader(*(char **)cmd_data);
                break;

                case BE_CMD_DRAW:
                    r_Draw(*(struct draw_command_buffer_t **)cmd_data);
                break;

                case BE_CMD_MEMCPY_TO:
                    memcpy_data = (struct memcpy_to_data_t *)cmd_data;
                    r_MemcpyTo(memcpy_data->dst, memcpy_data->src, memcpy_data->size);
                break;

                case BE_CMD_SWAP_BUFFERS:
                    SDL_GL_SwapWindow(r_renderer.window);
                break;
            }

            backend.next_out = (backend.next_out + 1) % BE_CMD_STREAM_BUFFER_SIZE;
        }
    }
}

void be_QueueCmd(int type, void *data, int size)
{
    SDL_AtomicLock(&backend.stream_spinlock);

    int in;
    int out;
    int diff;

    in = backend.next_in;
    out = backend.next_out;

    if(in < out)
    {
        in += BE_CMD_STREAM_BUFFER_SIZE;
    }

    diff = BE_CMD_STREAM_BUFFER_SIZE - abs(out - in) - 1;

    if(diff > 0 && backend.cmd_stream)
    {
        backend.cmd_stream[backend.next_in].type = type;

        if(data && size)
        {
            memcpy(backend.cmd_data_buffer + backend.next_in * BE_CMD_DATA_BUFFER_SIZE, data, size);
        }

        backend.next_in = (backend.next_in + 1) % BE_CMD_STREAM_BUFFER_SIZE;
    }

    SDL_AtomicUnlock(&backend.stream_spinlock);
}

void be_MatrixMode(int mode)
{
    //be_QueueCmd(BE_CMD_MATRIX_MODE, &mode, sizeof(int));
}

void be_LoadMatrix(float *matrix)
{
    //be_QueueCmd(BE_CMD_LOAD_MATRIX, matrix, sizeof(float) * 16);
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
}

void be_LoadTexture(char *file_name)
{
    //be_QueueCmd(BE_CMD_LOAD_TEXTURE, &file_name, sizeof(char *));
}

void be_MemcpyTo(struct verts_handle_t dst, void *src, unsigned int size)
{
    struct memcpy_to_data_t data;

    data.dst;
    data.src = src;
    data.size = size;

    be_QueueCmd(BE_CMD_MEMCPY_TO, &data, sizeof(data));
}

void be_Draw(struct draw_command_buffer_t *cmd_buffer)
{
    be_QueueCmd(BE_CMD_DRAW, &cmd_buffer, sizeof(cmd_buffer));
}

void be_WaitEmptyQueue()
{
    while(backend.next_in != backend.next_out);
}








