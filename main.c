#include "main.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[])
{
    mat3_t pitch_matrix;
    mat3_t yaw_matrix;
    vec2_t mouse_delta;

//    vec3_t tris[6] = {vec3_t(-1.0, 0.0, 1.0), vec3_t(1.0, 0.0, 1.0), vec3_t(1.0, 0.0, -1.0),
//                      vec3_t(1.0, 0.0, -1.0), vec3_t(-1.0, 0.0, -1.0), vec3_t(-1.0, 0.0, 1.0)};
//
//    vec3_t wall0[6] = {vec3_t(-1.0, 0.6, -1.0), vec3_t(-1.0, 0.0, -1.0), vec3_t(1.0, 0.0, -1.0),
//                      vec3_t(1.0, 0.0, -1.0), vec3_t(1.0, 0.6, -1.0), vec3_t(-1.0, 0.6, -1.0)};
//
//    vec3_t wall1[6] = {vec3_t(-1.0, 0.6, -1.0), vec3_t(-1.0, 0.6, 1.0), vec3_t(-1.0, 0.0, 1.0),
//                      vec3_t(-1.0, 0.0, 1.0), vec3_t(-1.0, 0.0, -1.0), vec3_t(-1.0, 0.6, -1.0)};

//    int x_count = 8;
//    int y_count = 8;
//    int y;
//    int x;
//    //int v;
//    vec3_t *verts;

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

    /*verts = (vec3_t *)calloc(sizeof(tris), 12000);

    for(x = 0, y = 0; y < 6; x++, y++)
    {
        verts[x * 6    ] = tris[0] + vec3_t(0.0, -0.5, -y * 2);
        verts[x * 6 + 1] = tris[1] + vec3_t(0.0, -0.5, -y * 2);
        verts[x * 6 + 2] = tris[2] + vec3_t(0.0, -0.5, -y * 2);
        verts[x * 6 + 3] = tris[3] + vec3_t(0.0, -0.5, -y * 2);
        verts[x * 6 + 4] = tris[4] + vec3_t(0.0, -0.5, -y * 2);
        verts[x * 6 + 5] = tris[5] + vec3_t(0.0, -0.5, -y * 2);
    }

    for(y = 0; y < 6; x++, y++)
    {
        verts[x * 6    ] = wall1[0] + vec3_t(0.0, -0.5, -y * 2);
        verts[x * 6 + 1] = wall1[1] + vec3_t(0.0, -0.5, -y * 2);
        verts[x * 6 + 2] = wall1[2] + vec3_t(0.0, -0.5, -y * 2);
        verts[x * 6 + 3] = wall1[3] + vec3_t(0.0, -0.5, -y * 2);
        verts[x * 6 + 4] = wall1[4] + vec3_t(0.0, -0.5, -y * 2);
        verts[x * 6 + 5] = wall1[5] + vec3_t(0.0, -0.5, -y * 2);
    }



    for(y = 0; y < 6; x++, y++)
    {
        verts[x * 6    ] = tris[0] + vec3_t(2.0, -0.6, -y * 2);
        verts[x * 6 + 1] = tris[1] + vec3_t(2.0, -0.6, -y * 2);
        verts[x * 6 + 2] = tris[2] + vec3_t(2.0, -0.6, -y * 2);
        verts[x * 6 + 3] = tris[3] + vec3_t(2.0, -0.6, -y * 2);
        verts[x * 6 + 4] = tris[4] + vec3_t(2.0, -0.6, -y * 2);
        verts[x * 6 + 5] = tris[5] + vec3_t(2.0, -0.6, -y * 2);
    }


    for(y = 0; y < 6; x++, y++)
    {
        verts[x * 6    ] = wall1[0] + vec3_t(2.0, -1.1, -y * 2);
        verts[x * 6 + 1] = wall1[1] + vec3_t(2.0, -1.1, -y * 2);
        verts[x * 6 + 2] = wall1[2] + vec3_t(2.0, -1.1, -y * 2);
        verts[x * 6 + 3] = wall1[3] + vec3_t(2.0, -1.1, -y * 2);
        verts[x * 6 + 4] = wall1[4] + vec3_t(2.0, -1.1, -y * 2);
        verts[x * 6 + 5] = wall1[5] + vec3_t(2.0, -1.1, -y * 2);
    }





    for(y = 0; y < 6; x++, y++)
    {
        verts[x * 6    ] = tris[0] + vec3_t(4.0, -1.2, -y * 2);
        verts[x * 6 + 1] = tris[1] + vec3_t(4.0, -1.2, -y * 2);
        verts[x * 6 + 2] = tris[2] + vec3_t(4.0, -1.2, -y * 2);
        verts[x * 6 + 3] = tris[3] + vec3_t(4.0, -1.2, -y * 2);
        verts[x * 6 + 4] = tris[4] + vec3_t(4.0, -1.2, -y * 2);
        verts[x * 6 + 5] = tris[5] + vec3_t(4.0, -1.2, -y * 2);
    }


    for(y = 0; y < 6; x++, y++)
    {
        verts[x * 6    ] = wall1[0] + vec3_t(4.0, -1.2, -y * 2);
        verts[x * 6 + 1] = wall1[1] + vec3_t(4.0, -1.2, -y * 2);
        verts[x * 6 + 2] = wall1[2] + vec3_t(4.0, -1.2, -y * 2);
        verts[x * 6 + 3] = wall1[3] + vec3_t(4.0, -1.2, -y * 2);
        verts[x * 6 + 4] = wall1[4] + vec3_t(4.0, -1.2, -y * 2);
        verts[x * 6 + 5] = wall1[5] + vec3_t(4.0, -1.2, -y * 2);
    }*/

//    for(y = 0; y < 6; x++, y++)
//    {
//        verts[x * 6    ] = wall1[0] + vec3_t(0.0, 0.0, -y * 2);
//        verts[x * 6 + 1] = wall1[1] + vec3_t(0.0, 0.0, -y * 2);
//        verts[x * 6 + 2] = wall1[2] + vec3_t(0.0, 0.0, -y * 2);
//        verts[x * 6 + 3] = wall1[3] + vec3_t(0.0, 0.0, -y * 2);
//        verts[x * 6 + 4] = wall1[4] + vec3_t(0.0, 0.0, -y * 2);
//        verts[x * 6 + 5] = wall1[5] + vec3_t(0.0, 0.0, -y * 2);
//    }

//    verts[0] = tris[0];
//    verts[1] = tris[1];
//    verts[2] = tris[2];
//
//    verts[3] = tris[3];
//    verts[4] = tris[4];
//    verts[5] = tris[5];
//
//    verts[6] = wall[0];
//    verts[7] = wall[1];
//    verts[8] = wall[2];
//
//    verts[9] = wall[3];
//    verts[10] = wall[4];
//    verts[11] = wall[5];

//    for(y = 0; y < y_count; y++)
//    {
//        for(x = 0; x < x_count; x++)
//        {
//            for(v = 0; v < 6; v++)
//            {
//                verts[y * x_count * 6 + x * 6 + v] = tris[v] + vec3_t(-x_count / 2 + x * 2, -1.0 + y * 0.5, -y_count / 2 + y * 2);
//            }
//        }
//    }
//
//    phy_BuildBvh(verts, 6 * (x_count * y_count));

//    verts = (vec3_t *)calloc(sizeof(vec3_t), 3);
//
//    verts[0] = vec3_t(5.0, -1.0, -5.0);
//    verts[1] = vec3_t(-5.0, -1.0, -5.0);
//    verts[2] = vec3_t(0.0, -1.0, 5.0);
//
    //phy_BuildBvh(verts, x * sizeof(tris));


    aux_LoadWavefront("test3.obj", &level_data);

    phy_BuildBvh((vec3_t *)level_data.vertices.buffer, level_data.vertices.cursor);



   // glEnable(GL_POINT_SMOOTH);
   // glPointSize(8.0);

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

    return 0;
}


