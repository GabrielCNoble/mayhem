#include "fe_frontend.h"
#include "be_backend.h"
#include "input.h"
#include "r_view.h"
#include "r_common.h"

#include "player.h"
#include "physics.h"

#include <stdio.h>

extern struct renderer_t r_renderer;

int fe_RunFrontend(void *data)
{
    struct view_t *view;

    while(1)
    {
        be_QueueCmd(BE_CMD_UPDATE_INPUT, NULL, 0);
        be_WaitEmptyQueue();

        player_UpdatePlayers();
        phy_UpdateColliders();
        player_PostUpdatePlayers();

        r_UpdateActiveView();

        //    glMatrixMode(GL_PROJECTION);
//    glLoadMatrixf((float *)r_renderer.active_view->projection_matrix.floats);
//
//    glMatrixMode(GL_MODELVIEW);
//    glLoadMatrixf((float *)r_renderer.active_view->view_matrix.floats);
       // view = r_GetActiveView();

        be_MatrixMode(GL_PROJECTION);
        be_LoadMatrix((float *)r_renderer.active_view->projection_matrix.floats);
        be_MatrixMode(GL_MODELVIEW);
        be_LoadMatrix((float *)r_renderer.active_view->view_matrix.floats);

        be_QueueCmd(BE_CMD_TEST, NULL, 0);
        be_QueueCmd(BE_CMD_SWAP_BUFFERS, NULL, 0);
        be_Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //be_QueueCmd(BE_CMD_CLEAR_BUFFER, NULL, 0);
        be_WaitEmptyQueue();

        if(in_GetKeyStatus(SDL_SCANCODE_ESCAPE) & KEY_STATUS_JUST_PRESSED)
        {
            be_QueueCmd(BE_CMD_SHUTDOWN, NULL, 0);
            break;
        }
    }
}
