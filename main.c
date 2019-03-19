#include "main.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[])
{
    SDL_Event event;

    struct view_t *view;
    mat3_t pitch_matrix;
    mat3_t yaw_matrix;
    vec2_t mouse_delta;

    float f = 0.0;

    float pitch = 0.0;
    float yaw = 0.0;

    int player_index;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("oh shit...\n");
        return -1;
    }

    r_Init();
    player_Init();
    phy_Init();

    r_SetWindowSize(800, 600);
    r_SetRendererResolution(800, 600);

    player_index = player_CreatePlayer("default", vec3_t(0.0, 0.0, 0.0));
    player_SetActivePlayer(player_index);

    in_RegisterKey(SDL_SCANCODE_W);
    in_RegisterKey(SDL_SCANCODE_S);
    in_RegisterKey(SDL_SCANCODE_A);
    in_RegisterKey(SDL_SCANCODE_D);
    in_RegisterKey(SDL_SCANCODE_SPACE);

    while(1)
    {
        in_UpdateInput();

        player_UpdatePlayers();
        phy_UpdateColliders();
        player_PostUpdatePlayers();

        r_UpdateActiveView();
        r_DrawFrame();
    }


    r_Shutdown();
}


