#ifndef R_FRAME_H
#define R_FRAME_H

#include "r_common.h"
#include "r_view.h"


void r_DrawWorld();

void r_DrawFrame();

void r_DrawLit(struct draw_command_buffer_t *cmd_buffer);

void r_DrawPortal(struct draw_command_buffer_t *cmd_buffer, int add);

void r_Clear();

#endif // R_FRAME_H
