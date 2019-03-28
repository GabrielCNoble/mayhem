#include "w_main.h"
#include "r_common.h"
#include "r_verts.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct verts_handle_t w_world_verts = R_INVALID_VERTS_HANDLE;
struct verts_handle_t w_world_indices = R_INVALID_VERTS_HANDLE;


struct draw_batch_t *w_world_batches = NULL;
unsigned int *w_world_indices_buffer = NULL;

void w_Init()
{

}

void w_Shutdown()
{

}

void w_LoadLevel(char *file_name)
{

}

void w_LoadLevelFromMemory(void *buffer)
{

}

#ifdef __cplusplus
}
#endif
