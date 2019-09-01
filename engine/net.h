#ifndef NET_H
#define NET_H

#include <stdint.h>

#define NET_PAYLOAD_BUFFER_SIZE 64

struct payload_buffer_t
{
    char data[NET_PAYLOAD_BUFFER_SIZE];
};

#ifdef __cplusplus
extern "C"
{
#endif

int32_t net_Init();

void net_Shutdown();

void net_Connect();

void net_Login(char *username, char *password);

uint32_t net_ConnectServer(char *ip, uint32_t port);

uint32_t net_ReconnectServer();

uint32_t net_SendServer(void *data, uint32_t length);

void net_ServerReceiveCallback(uint8_t type, void *data, uint32_t length);

void net_AddServerReceiveCallback(void (*callback)(uint8_t type, void *data, uint32_t length));

void net_QueueData(void *data, uint32_t size);

void *net_AllocPayloadBuffer(uint32_t size);

#ifdef __cplusplus
}
#endif




#endif // NET_H
