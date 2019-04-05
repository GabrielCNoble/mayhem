#include "main.h"
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

int main(int argc, char *argv[])
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL initialization has failed! reason\n%s", SDL_GetError());
        return -1;
    }

    r_Init();
    w_Init();
    player_Init();
    phy_Init();

    r_SetWindowSize(1300, 700);
    r_SetRendererResolution(1300, 700);

    be_Init();

    fe_RunFrontend();
    be_RunBackend();

    r_Shutdown();

    return 0;
}


