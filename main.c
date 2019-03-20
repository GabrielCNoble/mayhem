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

    vec3_t tris[6] = {vec3_t(-1.0, 0.0, 1.0), vec3_t(1.0, 0.0, 1.0), vec3_t(1.0, 0.0, -1.0),
                      vec3_t(1.0, 0.0, -1.0), vec3_t(-1.0, 0.0, -1.0), vec3_t(-1.0, 0.0, 1.0)};

    float f = 0.0;

    float pitch = 0.0;
    float yaw = 0.0;

    int x_count = 8;
    int y_count = 8;
    int y;
    int x;
    int v;
    vec3_t *verts;

    int player_index;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL initialization has failed! reason\n%s", SDL_GetError());
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
    in_RegisterKey(SDL_SCANCODE_ESCAPE);

    verts = (vec3_t *)calloc(sizeof(tris), x_count * y_count);

    for(y = 0; y < y_count; y++)
    {
        for(x = 0; x < x_count; x++)
        {
            for(v = 0; v < 6; v++)
            {
                verts[y * x_count * 6 + x * 6 + v] = tris[v] + vec3_t(-x_count / 2 + x * 2 + y, y, -y_count / 2 + y * 2);
            }
        }
    }

    phy_BuildBvh(verts, 6 * (x_count * y_count));

    glEnable(GL_POINT_SMOOTH);
    glPointSize(8.0);

    while(!(in_GetKeyStatus(SDL_SCANCODE_ESCAPE) & KEY_STATUS_PRESSED))
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


