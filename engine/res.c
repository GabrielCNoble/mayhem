#include "res.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL2/SDL_atomic.h"
#include "SDL2/SDL.h"

#include "../common/containers/ring_buffer.h"

struct ring_buffer_t res_cmd_stream;
unsigned char *res_cmd_data_buffer;
SDL_SpinLock stream_spinlock;

#define RES_RESOURCE_CMD_STREAM_BUFFER_SIZE 512
#define RES_RESOURCE_CMD_DATA_SIZE 64

void res_Init()
{
    res_cmd_stream.init(sizeof(struct resource_cmd_t), RES_RESOURCE_CMD_STREAM_BUFFER_SIZE);
    res_cmd_data_buffer = (unsigned char *)calloc(RES_RESOURCE_CMD_STREAM_BUFFER_SIZE, RES_RESOURCE_CMD_DATA_SIZE);
    SDL_Thread *resource_thread = SDL_CreateThread(res_ResourceQueue, "res_ResourceQueue", NULL);
    SDL_DetachThread(resource_thread);
}

void res_Shutdown()
{
    free(res_cmd_data_buffer);
}

void res_QueueCmd(int (*callback)(void *param), void *param, int size)
{
    SDL_AtomicLock(&stream_spinlock);

    struct resource_cmd_t cmd;
    int cmd_index;

    cmd.callback = callback;
    cmd_index = res_cmd_stream.add_next_no_advance(&cmd);

    if(param && size)
    {
        memcpy(res_cmd_data_buffer + cmd_index * RES_RESOURCE_CMD_DATA_SIZE, param, size);
    }

    res_cmd_stream.advance_next_in();

    SDL_AtomicUnlock(&stream_spinlock);
}

void res_NextCmd(struct resource_cmd_t *cmd, void **param)
{
    SDL_AtomicLock(&stream_spinlock);
    *cmd = *(struct resource_cmd_t *)res_cmd_stream.get_next_no_advance();
    *param = (void *)(res_cmd_data_buffer + res_cmd_stream.next_out * RES_RESOURCE_CMD_DATA_SIZE);
    SDL_AtomicUnlock(&stream_spinlock);
}

void res_AdvanceQueue()
{
    SDL_AtomicLock(&stream_spinlock);
    res_cmd_stream.advance_next_out();
    SDL_AtomicUnlock(&stream_spinlock);
}

int res_ResourceQueue(void *params)
{
    struct resource_cmd_t cmd;
    void *param;

    SDL_Thread *loader_thread;

    while(1)
    {
        if(res_cmd_stream.next_in != res_cmd_stream.next_out)
        {
            res_NextCmd(&cmd, &param);

            if(cmd.callback)
            {
                loader_thread = SDL_CreateThread(cmd.callback, "loader_thread", param);
//                cmd.callback(param);
            }

            res_AdvanceQueue();
        }
        /* ~30Hz... */
        SDL_Delay(33);
    }
}





