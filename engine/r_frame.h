#ifndef R_FRAME_H
#define R_FRAME_H

#include "r_common.h"
#include "r_view.h"


enum R_DEBUG_DRAW
{
    R_DEBUG_DRAW_OPTION_LIGHTS = 1,
    R_DEBUG_DRAW_OPTION_COLLIDERS = 1 << 1,
    R_DEBUG_DRAW_OPTION_DBVH = 1 << 2,
};

#ifdef __cplusplus
extern "C"
{
#endif

void r_InitFrame();

void r_ShutdownFrame();

//void r_DrawDebug(struct draw_command_buffer_t *cmd_buffer, int options);

//void r_DrawLine(struct draw_command_buffer_t *cmd_buffer);

void r_DrawLit(struct draw_command_buffer_t *cmd_buffer);

void r_PortalStencil(struct draw_command_buffer_t *cmd_buffer, int remove);

void r_DrawRaytracedShadows(struct draw_command_buffer_t *cmd_buffer);

void r_Clear(int mask);

void r_be_Clear(int mask);

void r_SwapBuffers();

void r_i_SwapBuffers();

#ifdef __cplusplus
}
#endif

#endif // R_FRAME_H
