#ifndef S_BACKEND_H
#define S_BACKEND_H


#include "r_verts.h"



enum BE_BACKEND_CMD
{
    BE_CMD_SWAP_BUFFERS = 0,
    BE_CMD_CLEAR_BUFFER,

    BE_CMD_SET_SHADER,
    BE_CMD_SET_TEXTURE,

    BE_CMD_LOAD_SHADER,
    BE_CMD_LOAD_TEXTURE,
    BE_CMD_UPLOAD_TEXTURE,
    BE_CMD_MEMCPY_TO,
    BE_CMD_UPLOAD_LIGHTS,
    BE_CMD_UPLOAD_TRIANGLES,
    BE_CMD_UPLOAD_TRIANGLE_INDICES,

    BE_CMD_DRAW_LIT,
    BE_CMD_DRAW_DBG,
    BE_CMD_DRAW_LINE,
    BE_CMD_DRAW_TRIANGLE,
    BE_CMD_DRAW_CAPSULE,
    BE_CMD_DRAW_POINT,
    BE_CMD_ADD_PORTAL_STENCIL,
    BE_CMD_REM_PORTAL_STENCIL,

    BE_CMD_ENABLE_VERTS_READS,

    BE_CMD_UPDATE_INPUT,
    BE_CMD_SHUTDOWN,
    BE_CMD_TEST,
};

struct backend_cmd_t
{
    unsigned int type;
};

void be_Init();

void be_Shutdown();

void be_RunBackend();


void be_QueueCmd(int type, void *data, int size);

void be_NextCmd(struct backend_cmd_t *cmd, unsigned char **cmd_data);

void be_AdvanceQueue();

void be_Clear(int mask);

void be_SwapBuffers();

void be_SetShader(int shader_handle);

void be_LoadShader(char *file_name);

struct texture_handle_t be_LoadTexture(char *file_name);

void be_MemcpyTo(struct verts_handle_t dst, void *src, unsigned int size);

void be_DrawDebug(struct draw_command_buffer_t *cmd_buffer);

void be_DrawLit(struct draw_command_buffer_t *cmd_buffer);

void be_AddPortalStencil(struct draw_command_buffer_t *cmd_buffer);

void be_RemPortalStencil(struct draw_command_buffer_t *cmd_buffer);

void be_UploadLights(struct light_buffer_t *light_buffer);

void be_UploadTriangles(vec3_t *vertices, int count);

void be_UploadTriangleIndices(unsigned int *indices, int count);

void be_UploadWorldTris();

void be_EnableVertsReads();

void be_WaitEmptyQueue();

void *be_WaitLastResult();




#endif // S_BACKEND_H
