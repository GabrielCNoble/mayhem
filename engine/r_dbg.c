#include "r_dbg.h"
#include "r_verts.h"
#include "r_view.h"
#include "be_backend.h"
#include "be_r_dbg.h"
#include "../common/containers/stack_list.h"

struct verts_handle_t r_debug_verts_buffer_handle;
unsigned int r_debug_verts_buffer_start;
//struct stack_list_t r_debug_data_buffer;

//#define R_DEBUG_DATA_BUFFER_SIZE 128

void r_InitDebug()
{
    r_debug_verts_buffer_handle = r_AllocVerts(sizeof(struct vertex_t) * 3 * 512, sizeof(struct vertex_t));
    r_debug_verts_buffer_start = r_GetAllocStart(r_debug_verts_buffer_handle) / sizeof(struct vertex_t);
//    r_debug_data_buffer.init(R_DEBUG_DATA_BUFFER_SIZE, 32);
}

void r_ShutdownDebug()
{
    r_Free(r_debug_verts_buffer_handle);
}

void r_DrawLine(vec3_t a, vec3_t b, vec3_t color)
{
    struct draw_line_data_t data;

    data.a = a;
    data.b = b;
    data.color = color;

    be_QueueCmd(BE_CMD_DRAW_LINE, &data, sizeof(data));
}

void r_DrawTriangle(vec3_t a, vec3_t b, vec3_t c, vec3_t color, int wireframe, int backface_culling, int depth_test)
{
    struct draw_triangle_data_t data;

    data.a = a;
    data.b = b;
    data.c = c;
    data.color = color;
    data.wireframe = wireframe && 1;
    data.backface_culling = backface_culling && 1;
    data.depth_test = depth_test && 1;

    be_QueueCmd(BE_CMD_DRAW_TRIANGLE, &data, sizeof(data));
}

void r_DrawCapsule(vec3_t position, float height, float radius)
{
    struct draw_capsule_data_t data;

    data.position = position;
    data.height = height;
    data.radius = radius;

    be_QueueCmd(BE_CMD_DRAW_CAPSULE, &data, sizeof(data));
}

void r_DrawPoint(vec3_t position, vec3_t color, float size, int smooth)
{
    struct draw_point_data_t data;

    data.position = position;
    data.color = color;
    data.size = size;
    data.smooth = smooth;

    be_QueueCmd(BE_CMD_DRAW_POINT, &data, sizeof(data));
}

void r_DrawDebug(struct view_t *view)
{
    struct draw_debug_data_t data;

    data.view_projection_matrix = view->view_projection_matrix;

    be_QueueCmd(BE_CMD_DRAW_DBG, &data, sizeof(data));
}






