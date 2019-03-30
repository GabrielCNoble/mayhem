#include "be_backend.h"
#include "r_common.h"
#include "r_frame.h"
#include "input.h"
#include "SDL2/SDL_atomic.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>




#define BE_BACKEND_CMD_STREAM_BUFFER_SIZE 1024
#define BE_BACKEND_CMD_DATA_BUFFER_SIZE 64

int next_in = 0;
int next_out = 0;
SDL_SpinLock stream_spinlock;

struct backend_cmd_t *cmd_stream;
unsigned char *cmd_data_buffer;
extern struct renderer_t r_renderer;


void be_Init()
{
    cmd_stream = (struct backend_cmd_t *)calloc(sizeof(struct backend_cmd_t), BE_BACKEND_CMD_STREAM_BUFFER_SIZE);
    cmd_data_buffer = (unsigned char *)calloc(BE_BACKEND_CMD_DATA_BUFFER_SIZE, BE_BACKEND_CMD_STREAM_BUFFER_SIZE);
    SDL_AtomicUnlock(&stream_spinlock);
}

void be_Shutdown()
{
    free(cmd_stream);
}

void be_RunBackend()
{
    struct backend_cmd_t cmd;

    void *data;
    int matrix_mode;
    int mask;

    while(1)
    {
        if(next_out != next_in)
        {
            SDL_AtomicLock(&stream_spinlock);
            cmd = cmd_stream[next_out];
            data = cmd_data_buffer + next_out * BE_BACKEND_CMD_DATA_BUFFER_SIZE;
            SDL_AtomicUnlock(&stream_spinlock);

            switch(cmd.type)
            {
                case BE_CMD_SHUTDOWN:
                    return;
                break;

                case BE_CMD_UPDATE_INPUT:
                    in_UpdateInput();
                break;

                case BE_CMD_TEST:
                    r_DrawFrame();
                break;

                case BE_CMD_MATRIX_MODE:
                    matrix_mode = *(int *)data;
                    glMatrixMode(matrix_mode);
                break;

                case BE_CMD_LOAD_MATRIX:
                    glLoadMatrixf((const float*) data);
                break;

                case BE_CMD_CLEAR_BUFFER:
                    mask = *(int *)data;
                    glClear(mask);
                break;

                case BE_CMD_SWAP_BUFFERS:
                    SDL_GL_SwapWindow(r_renderer.window);
                break;
            }

            next_out = (next_out + 1) % BE_BACKEND_CMD_STREAM_BUFFER_SIZE;
        }
    }
}

void be_QueueCmd(int type, void *data, int size)
{
    SDL_AtomicLock(&stream_spinlock);

    int in;
    int out;
    int diff;

    in = next_in;
    out = next_out;

    if(in < out)
    {
        in += BE_BACKEND_CMD_STREAM_BUFFER_SIZE;
    }

    diff = BE_BACKEND_CMD_STREAM_BUFFER_SIZE - abs(out - in) - 1;

    if(diff > 0)
    {
        cmd_stream[next_in].type = type;

        if(data && size)
        {
            memcpy(cmd_data_buffer + next_in * BE_BACKEND_CMD_DATA_BUFFER_SIZE, data, size);
        }

        next_in = (next_in + 1) % BE_BACKEND_CMD_STREAM_BUFFER_SIZE;
    }

    SDL_AtomicUnlock(&stream_spinlock);
}

void be_MatrixMode(int mode)
{
    be_QueueCmd(BE_CMD_MATRIX_MODE, &mode, sizeof(int));
}

void be_LoadMatrix(float *matrix)
{
    be_QueueCmd(BE_CMD_LOAD_MATRIX, matrix, sizeof(float) * 16);
}

void be_Clear(int mask)
{
    be_QueueCmd(BE_CMD_CLEAR_BUFFER, &mask, sizeof(int));
}

void be_WaitEmptyQueue()
{
    while(next_in != next_out);
}








