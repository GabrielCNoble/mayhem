#ifndef R_MAIN_H
#define R_MAIN_H

#include <stdint.h>

#include "r_common.h"
#include "r_frame.h"
#include "r_shader.h"
#include "r_light.h"
#include "r_surf.h"
#include "r_gui.h"
#include "r_dbg.h"
#include "r_verts.h"

enum R_CMD
{
    R_CMD_SWAP_BUFFERS = 0,
    R_CMD_CLEAR_BUFFER,

    R_CMD_SET_SHADER,
    R_CMD_SET_TEXTURE,

    R_CMD_LOAD_SHADER,
    R_CMD_LOAD_TEXTURE,
    R_CMD_UPLOAD_TEXTURE,
    R_CMD_MEMCPY_TO,
    R_CMD_UPLOAD_LIGHTS,
    R_CMD_UPLOAD_TRIANGLES,
    R_CMD_UPLOAD_TRIANGLE_INDICES,

    R_CMD_DRAW_LIT,
    R_CMD_DRAW_DBG,
    R_CMD_DRAW_LINE,
    R_CMD_DRAW_TRIANGLE,
    R_CMD_DRAW_CAPSULE,
    R_CMD_DRAW_POINT,
    R_CMD_ADD_PORTAL_STENCIL,
    R_CMD_REM_PORTAL_STENCIL,

    R_CMD_DRAW_GUI,

    R_CMD_ENABLE_VERTS_READS,

    R_CMD_UPDATE_INPUT,
    R_CMD_SHUTDOWN,
    R_CMD_TEST,
};

struct r_cmd_t
{
    unsigned int type;
};


#ifdef __cplusplus
extern "C"
{
#endif

int32_t r_Init();

void r_Shutdown();

void r_SetWindowSize(int width, int height);

void r_SetRendererResolution(int width, int height);

struct framebuffer_t r_CreateFramebuffer();

void r_AddFramebufferAttachment(struct framebuffer_t *framebuffer, int attachment, int format, int internal_format, int type);

void r_SetFramebufferResolution(struct framebuffer_t *framebuffer, int width, int height);

void r_Main();

void r_QueueCmd(int type, void *data, int size);

void r_NextCmd(struct r_cmd_t *cmd, unsigned char **cmd_data);

void r_AdvanceQueue();

//void be_Clear(int mask);

//void be_SwapBuffers();

//void be_SetShader(int shader_handle);

//void be_LoadShader(char *file_name);

//struct texture_handle_t be_LoadTexture(char *file_name);

void be_MemcpyTo(struct verts_handle_t dst, void *src, unsigned int size);

void be_DrawDebug(struct draw_command_buffer_t *cmd_buffer);

void be_DrawLit(struct draw_command_buffer_t *cmd_buffer);

void be_AddPortalStencil(struct draw_command_buffer_t *cmd_buffer);

void be_RemPortalStencil(struct draw_command_buffer_t *cmd_buffer);

void be_UploadLights(struct light_buffer_t *light_buffer);

//void be_UploadTriangles(vec3_t *vertices, int count);

//void be_UploadTriangleIndices(unsigned int *indices, int count);

//void be_UploadWorldTris();

//void be_EnableVertsReads();

void r_WaitEmptyQueue();

//void *be_WaitLastResult();

#ifdef __cplusplus
}
#endif


#endif // R_MAIN_H
