#include "main.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[])
{
    struct geometry_data_t level_data;

    int player_index;

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

    player_index = player_CreatePlayer("default", vec3_t(3.0, 0.22, 0.0));
    player_SetActivePlayer(player_index);

    in_RegisterKey(SDL_SCANCODE_W);
    in_RegisterKey(SDL_SCANCODE_S);
    in_RegisterKey(SDL_SCANCODE_A);
    in_RegisterKey(SDL_SCANCODE_D);
    in_RegisterKey(SDL_SCANCODE_SPACE);
    in_RegisterKey(SDL_SCANCODE_ESCAPE);

    aux_LoadWavefront("test2.obj", &level_data);

    phy_BuildBvh((vec3_t *)level_data.vertices.buffer, level_data.vertices.cursor);

    be_Init();

    SDL_Thread *thread;
    thread = SDL_CreateThread(fe_RunFrontend, "Frontend", NULL);

    be_RunBackend();
    be_Shutdown();

    //

   // glEnable(GL_POINT_SMOOTH);
   // glPointSize(8.0);

   //thread = SDL_CreateThread(thread_fn, "Thread", NULL);

//    while(!(in_GetKeyStatus(SDL_SCANCODE_ESCAPE) & KEY_STATUS_PRESSED))
//    {
//        in_UpdateInput();
//
//        player_UpdatePlayers();
//        phy_UpdateColliders();
//        player_PostUpdatePlayers();
//
//        r_UpdateActiveView();
//
//        r_DrawFrame();
//    }


    r_Shutdown();

    return 0;
}


