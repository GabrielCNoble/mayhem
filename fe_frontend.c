#include "fe_frontend.h"
#include "be_backend.h"
#include "input.h"
#include "r_view.h"
#include "r_verts.h"
#include "r_common.h"
#include "r_shader.h"
#include "r_portal.h"
#include "r_light.h"
#include "r_vis.h"
#include "s_aux.h"
#include "w_main.h"

#include "player.h"
#include "physics.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern struct renderer_t r_renderer;
extern struct list_t w_world_batches;

int fe_Frontend(void *data)
{
    //struct view_t *view;
    //struct draw_batch_t *batch;
    //struct geometry_data_t level_data;
    int player_index;

  //  struct draw_command_buffer_t cmd_buffer;
  //  struct draw_command_t draw_cmd;

 //   struct vertex_t *vertices;
 //   struct verts_handle_t handle;

   // int shd = fe_LoadShader("shaders/test");

    player_index = player_CreatePlayer("default", vec3_t(3.0, 0.22, 0.0));
    player_SetActivePlayer(player_index);

    player_CreatePlayer("default1", vec3_t(4.0, 0.22, 0.0));
    player_CreatePlayer("default2", vec3_t(-3.0, 0.22, 0.0));
    player_CreatePlayer("default2", vec3_t(-3.0, 2.22, 5.0));

    in_RegisterKey(SDL_SCANCODE_W);
    in_RegisterKey(SDL_SCANCODE_S);
    in_RegisterKey(SDL_SCANCODE_A);
    in_RegisterKey(SDL_SCANCODE_D);
    in_RegisterKey(SDL_SCANCODE_SPACE);
    in_RegisterKey(SDL_SCANCODE_ESCAPE);

    w_LoadLevel("test4.obj");

//    cmd_buffer.draw_commands.init(sizeof(struct draw_command_t), 32);

//    for(i = 0; i < w_world_batches.cursor; i++)
//    {
//        batch = (struct draw_batch_t *)w_world_batches.get(i);
//
//        draw_cmd.batch.start = batch->start;
//        draw_cmd.batch.count = batch->count;
//        draw_cmd.batch.material = batch->material;
//        draw_cmd.transform = NULL;
//
//        cmd_buffer.draw_commands.add(&draw_cmd);
//    }

    r_CreateLight(vec3_t(5.0, 1.2, 0.0), vec3_t(1.0, 1.0, 1.0), 0.2, 20.0);
    r_CreateLight(vec3_t(-5.0, 1.2, -3.0), vec3_t(1.0, 0.4, 0.2), 0.3, 20.0);
    r_CreateLight(vec3_t(0.0, 8.2, 1.5), vec3_t(0.2, 0.4, 1.0), 0.3, 20.0);

    int portal0 = r_CreatePortal(vec3_t(1.8, 1.2, 7.5), vec2_t(1.5, 2.4));
    int portal1 = r_CreatePortal(vec3_t(3.8, 1.2, -7.5), vec2_t(1.5, 2.4));
    r_LinkPortals(portal0, portal1);

    int portal2 = r_CreatePortal(vec3_t(3.8, 1.2, 7.5), vec2_t(1.5, 2.4));
    int portal3 = r_CreatePortal(vec3_t(0.0, 8.5, 0.6), vec2_t(1.5, 2.4));
    r_LinkPortals(portal2, portal3);


    struct portal_t *portal;


    portal = r_GetPortalPointer(portal0);
    portal->orientation = rotate_y(-0.5);

    portal = r_GetPortalPointer(portal1);
    portal->orientation = rotate_y(0.5);

    portal = r_GetPortalPointer(portal2);
    portal->orientation = rotate_y(0.5);
//
    portal = r_GetPortalPointer(portal3);
    portal->orientation = rotate_y(1.0);


    //portal->orientation = rotate_x(-0.5);
    //portal->orientation = rotate(vec3_t(1.0, 0.0, 0.0), -0.5);


    float pos = 2.5;
    float angle = 0.0;

    while(1)
    {
        //portal->orientation = rotate_y(portal->orientation, angle);

      /*  portal->position.y = pos + sin(angle) * 2.0;
        angle += 0.009;*/

        be_QueueCmd(BE_CMD_UPDATE_INPUT, NULL, 0);
        be_WaitEmptyQueue();

        player_UpdatePlayers();
        phy_UpdateColliders();
        player_PostUpdatePlayers();

        be_Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        r_VisibleWorld();
        be_SwapBuffers();
        be_WaitEmptyQueue();

        if(in_GetKeyStatus(SDL_SCANCODE_ESCAPE) & KEY_STATUS_JUST_PRESSED)
        {
            be_QueueCmd(BE_CMD_SHUTDOWN, NULL, 0);
            break;
        }
    }

    return 0;
}

void fe_RunFrontend()
{
    SDL_Thread *frontend_thread;
    frontend_thread = SDL_CreateThread(fe_Frontend, "Frontend", NULL);
    SDL_DetachThread(frontend_thread);
}

int fe_LoadShader(char *file_name)
{
    be_LoadShader(file_name);
    be_WaitEmptyQueue();
    return r_GetShader(file_name);
}




