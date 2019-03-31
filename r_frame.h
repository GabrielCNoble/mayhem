#ifndef R_FRAME_H
#define R_FRAME_H

#include "r_common.h"
#include "r_view.h"


void r_DrawWorld();

void r_DrawFrame();

void r_Draw(struct draw_command_buffer_t *cmd_buffer);

void r_MatrixMode(int mode);

void r_Clear();

#endif // R_FRAME_H
