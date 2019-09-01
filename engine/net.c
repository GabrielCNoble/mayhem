#include "net.h"
#include "SDL2\SDL.h"
#include "Windows.h"
#include "console.h"
#include "../common/containers/ring_buffer.h"

#include <stdio.h>

int (*net_ConnectFn)(char *) = NULL;
void (*net_LoginFn)() = NULL;
uint32_t (*net_ConnectServerFn)(char *ip, uint32_t port) = NULL;
uint32_t (*net_SendServerFn)(void *data, uint32_t length) = NULL;
void (*net_AddServerReceiveCallbackFn)(void (*callback)(uint8_t type, void *data, uint32_t length)) = NULL;


void *net_client_lib = NULL;

struct ring_buffer_t net_payload_buffer;

#ifdef __cplusplus
extern "C"
{
#endif

int32_t net_Init()
{

//    HMODULE handle = LoadLibraryA("balls.dll");

    net_client_lib = SDL_LoadObject("GameClient.dll");

    if(!net_client_lib)
    {
        printf("Couldn't load net client dll. %s", SDL_GetError());
        return 1;
    }

    net_ConnectFn = (int32_t (*)(char *))SDL_LoadFunction(net_client_lib, "connect");
    net_ConnectServerFn = (uint32_t (*)(char *, uint32_t))SDL_LoadFunction(net_client_lib, "connect_server");
    net_SendServerFn = (uint32_t (*)(void *, uint32_t))SDL_LoadFunction(net_client_lib, "send_server");
    net_AddServerReceiveCallbackFn = (void (*)(void (*callback)(uint8_t type, void *data, uint32_t length)))SDL_LoadFunction(net_client_lib, "on_server_receive");

    con_CreateCVar("connect", CVAR_TYPE_FUNCTION, (void *)net_Connect);
    con_CreateCVar("connect_server", CVAR_TYPE_FUNCTION, (void *)net_ConnectServer);
    con_CreateCVar("send_server", CVAR_TYPE_FUNCTION, (void *)net_SendServer);
    con_CreateCVar("reconnect_server", CVAR_TYPE_FUNCTION, (void *)net_ReconnectServer);

    net_AddServerReceiveCallback(net_ServerReceiveCallback);
    net_ConnectServer("192.168.100.105", 64001);

    net_payload_buffer.init(sizeof(struct payload_buffer_t), 1024);

    return 0;
}

void net_Shutdown()
{
    SDL_UnloadObject(net_client_lib);
}


void net_Connect()
{
    if(net_ConnectFn)
    {
        if(net_ConnectFn("www.turconi.com.br"))
        {
            con_Printf("Connection failed!");
        }
        else
        {
            con_Printf("Connection succesful!");
        }
    }
    else
    {
        con_Printf("Nope!");
    }
}

void net_Login(char *username, char *password)
{
}

uint32_t net_ConnectServer(char *ip, uint32_t port)
{
    if(net_ConnectServerFn)
    {
        if(!net_ConnectServerFn(ip, port))
        {
            con_Printf("net_ConnectServer: connection successful!");
        }
        else
        {
            con_Printf("net_ConnectServer: connection failed!");
        }
    }
    else
    {
        con_Printf("net_ConnectServer: null function pointer!");
    }
}

uint32_t net_ReconnectServer()
{
    net_ConnectServer("192.168.100.105", 64001);
}

uint32_t net_SendServer(void *data, uint32_t length)
{
//    char *message = "FUCK THIS SHIT!";

    if(net_SendServerFn)
    {
        net_SendServerFn(data, length);
    }
    else
    {
        con_Printf("net_SendServer: null function pointer!");
    }
}

void net_ServerReceiveCallback(uint8_t type, void *data, uint32_t length)
{
    static char str[512];
    uint32_t i;

    for(i = 0; i < length; i++)
    {
        str[i] = ((char *)data)[i];
    }

    str[i] = '\0';

    con_Printf(str);
}

void net_AddServerReceiveCallback(void (*callback)(uint8_t type, void *data, uint32_t length))
{
    if(net_AddServerReceiveCallbackFn)
    {
        net_AddServerReceiveCallbackFn(callback);
    }
    else
    {
        con_Printf("net_AddServerReceiveCallback: null function pointer!");
    }
}

void *net_AllocPayloadBuffer(uint32_t size)
{
    uint32_t buffer_count = 1 + (size / NET_PAYLOAD_BUFFER_SIZE);
    void *buffer;

    net_payload_buffer.add_next(NULL);
    buffer = net_payload_buffer.get_next();

    for(buffer_count--; buffer_count > 0; buffer_count--)
    {
        net_payload_buffer.add_next(NULL);
        net_payload_buffer.get_next();
    }

    return buffer;
}

#ifdef __cplusplus
}
#endif







