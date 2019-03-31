#include "fe_frontend.h"
#include "be_backend.h"
#include "input.h"
#include "r_view.h"
#include "r_verts.h"
#include "r_common.h"
#include "r_shader.h"
#include "s_aux.h"

#include "player.h"
#include "physics.h"

#include <stdio.h>
#include <stdlib.h>

extern struct renderer_t r_renderer;

int fe_Frontend(void *data)
{
    int i;
    struct view_t *view;
    struct draw_batch_t *batch;
    struct geometry_data_t level_data;
    int player_index;

    struct draw_command_buffer_t cmd_buffer;
    struct draw_command_t draw_cmd;

    struct vertex_t *vertices;
    struct verts_handle_t handle;

    int shd = fe_LoadShader("shaders/test");

    player_index = player_CreatePlayer("default", vec3_t(3.0, 0.22, 0.0));
    player_SetActivePlayer(player_index);

    in_RegisterKey(SDL_SCANCODE_W);
    in_RegisterKey(SDL_SCANCODE_S);
    in_RegisterKey(SDL_SCANCODE_A);
    in_RegisterKey(SDL_SCANCODE_D);
    in_RegisterKey(SDL_SCANCODE_SPACE);
    in_RegisterKey(SDL_SCANCODE_ESCAPE);

    aux_LoadWavefront("test4.obj", &level_data);

    phy_BuildBvh((vec3_t *)level_data.vertices.buffer, level_data.vertices.cursor);

    vertices = (struct vertex_t *)calloc(sizeof(vertex_t), level_data.vertices.cursor);

    for(i = 0; i < level_data.vertices.cursor; i++)
    {
        vertices[i].position = *(vec3_t *)level_data.vertices.get(i);
        vertices[i].normal = *(vec3_t *)level_data.normals.get(i);
    }

    handle = r_AllocVerts(sizeof(vertex_t) * level_data.vertices.cursor);

    be_MemcpyTo(handle, vertices, sizeof(vertex_t) * level_data.vertices.cursor);

    cmd_buffer.draw_commands.init(sizeof(struct draw_command_t), 32);

    for(i = 0; i < level_data.draw_batches.cursor; i++)
    {
        batch = (struct draw_batch_t *)level_data.draw_batches.get(i);

        draw_cmd.batch.start = batch->start;
        draw_cmd.batch.count = batch->count;
        draw_cmd.batch.material = batch->material;
        draw_cmd.transform = NULL;

        cmd_buffer.draw_commands.add(&draw_cmd);
    }

    cmd_buffer.view = r_GetActiveView();

    while(1)
    {
        be_QueueCmd(BE_CMD_UPDATE_INPUT, NULL, 0);
        be_WaitEmptyQueue();


        player_UpdatePlayers();
        phy_UpdateColliders();
        player_PostUpdatePlayers();

        r_UpdateActiveView();

        be_SetShader(shd);

        be_Draw(&cmd_buffer);
        be_SwapBuffers();
        be_Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        be_WaitEmptyQueue();

        if(in_GetKeyStatus(SDL_SCANCODE_ESCAPE) & KEY_STATUS_JUST_PRESSED)
        {
            be_QueueCmd(BE_CMD_SHUTDOWN, NULL, 0);
            break;
        }
    }
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




