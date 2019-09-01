#ifndef RES_H
#define RES_H

struct resource_cmd_t
{
    int (*callback)(void *param);
};

enum RES_RESOURCE_CMD
{
    RES_RESOURCE_CMD_LOAD = 1,
    RES_RESOURCE_CMD_SHUTDOWN,
};

#ifdef __cplusplus
extern "C"
{
#endif

void res_Init();

void res_Shutdown();

void res_QueueCmd(int (*callback)(void *param), void *param, int size);

void res_NextCmd(struct resource_cmd_t *cmd, void **param);

void res_AdvanceQueue();

int res_ResourceQueue(void *params);

#ifdef __cplusplus
}
#endif


#endif // RES_H
