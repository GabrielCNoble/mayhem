#include "net.h"
#include "SDL2\SDL.h"
#include "Windows.h"

#include <stdio.h>

void (*net_ConnectFn)() = NULL;
void (*net_LoginFn)() = NULL;

void *net_client_lib = NULL;

void net_Init()
{

    HMODULE handle = LoadLibraryA("balls.dll");
//    net_client_lib = SDL_LoadObject("GameClient.dll");

    if(!handle)
    {
        printf("FUUUUCK\n");
    }
//    else
//    {
//        printf("net_Init: %s\n", SDL_GetError());
//    }
}

void net_Shutdown()
{

}


void net_Connect()
{

}

void net_Login(char *username, char *password)
{

}
