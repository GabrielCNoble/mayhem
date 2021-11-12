#include "main.h"
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

void init_callback()
{
    struct player_handle_t player;

    player = player_CreatePlayer("default", vec3_t(-1.23, 2.6, 0.0), vec3_t(0.0, 0.3, 0.0), 0);
    player_SetActivePlayer(player);

    in_RegisterKey(SDL_SCANCODE_W);
    in_RegisterKey(SDL_SCANCODE_S);
    in_RegisterKey(SDL_SCANCODE_A);
    in_RegisterKey(SDL_SCANCODE_D);
    in_RegisterKey(SDL_SCANCODE_E);
    in_RegisterKey(SDL_SCANCODE_SPACE);
    in_RegisterKey(SDL_SCANCODE_ESCAPE);
    in_RegisterKey(SDL_SCANCODE_LCTRL);

    r_CreateLight(vec3_t(5.0, 0.9, 0.0), vec3_t(1.0, 1.0, 1.0), 0.2, 20.0);
    r_CreateLight(vec3_t(6.0, 1.5, 6.0), vec3_t(1.0, 1.0, 1.0), 0.2, 20.0);
    r_CreateLight(vec3_t(0.0, 4.2, 2.0), vec3_t(1.0, 1.0, 1.0), 0.2, 20.0);
    r_CreateLight(vec3_t(-5.0, 1.2, -3.0), vec3_t(0.0, 0.2, 0.4), 0.3, 20.0);
    r_CreateLight(vec3_t(0.0, 8.2, 1.5), vec3_t(1.0, 0.2, 0.4), 0.3, 20.0);
    r_CreateLight(vec3_t(0.0, -12.0, 0.0), vec3_t(1.0, 1.0, 1.0), 0.3, 20.0);
    r_CreateLight(vec3_t(-20.0, -2.0, 0.0), vec3_t(1.0, 1.0, 1.0), 0.3, 20.0);

    int portal0 = r_CreatePortal(vec3_t(3.0, 10.0, 2.0), vec2_t(3.0, 4.0));
    int portal1 = r_CreatePortal(vec3_t(7.0, 2.0, 2.0), vec2_t(3.0, 4.0));
    struct portal_t *portal = r_GetPortalPointer(portal1);
    rotate_y(portal->orientation, 0.5);
    rotate_z(portal->orientation, -0.1);
    portal = r_GetPortalPointer(portal0);
//    rotate_y(portal->orientation, -0.5);
    rotate_x(portal->orientation, -0.5);
    r_LinkPortals(portal0, portal1);

//    int portal2 = r_CreatePortal(vec3_t(9.0, 2.0, -2.0), vec2_t(3.0, 4.0));
//    int portal3 = r_CreatePortal(vec3_t(-5.0, 2.0, -0.0), vec2_t(3.0, 4.0));
//    portal = r_GetPortalPointer(portal2);
//    rotate_y(portal->orientation, 0.5);
//    portal = r_GetPortalPointer(portal3);
//    rotate_y(portal->orientation, -0.7);
//    r_LinkPortals(portal2, portal3);

    w_LoadLevel("../models/level0.gmy");
}

int main(int argc, char *argv[])
{
//    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
//    {
//        printf("SDL initialization has failed! reason\n%s", SDL_GetError());
//        return -1;
//    }

//    r_Init();
//    gui_Init();
//    con_Init();
//    w_Init();
//    ent_Init();
//    player_Init();
//    phy_Init();
//    res_Init();
//    net_Init();

    if(sys_Init() >= 0)
    {
        sys_SetInitCallback(init_callback);
        sys_MainLoop();
    }

//    r_SetWindowSize(800, 600);
//    r_SetRendererResolution(800, 600);

//    be_Init();
//    fe_RunFrontend();
//    r_Main();
//    gui_Shutdown();
//    r_Shutdown();

    return 0;
}


