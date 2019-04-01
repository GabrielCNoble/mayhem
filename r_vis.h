#ifndef R_VIS_H
#define R_VIS_H

#include "r_common.h"


void r_VisibleWorld();

void r_VisibleWorldOnView(struct view_t *view);

void r_VisibleWorldTrianglesOnView(struct view_t *view, struct draw_command_buffer_t *cmd_buffer);

void r_TraverseBvh(struct bvh_node_t *node, struct draw_batch_t *batches);


struct draw_command_buffer_t *r_AllocCommandBuffer();

void r_ReclaimCommandBuffers();

#endif // R_VIS_H
