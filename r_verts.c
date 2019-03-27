#include "r_verts.h"
#include "stack_list.h"
#include "list.h"

struct gpu_chunk_t
{
    unsigned int size;
    unsigned int start;
}



struct stack_list_t r_vert_allocs(sizeof(struct gpu_chunk_t), 32);
struct list_t r_free_verts(sizeof(struct gpu_chunk_t), 32);


struct stack_list_t r_index_allocs(sizeof(struct gpu_chunk_t), 32);
struct list_t r_free_indexes(sizeof(struct gpu_chunk_t), 32);


struct verts_handle_t r_AllocVert(int size)
{

}

struct verts_handle_t r_AllocIndex(int size)
{

}

struct verts_handle_t r_Alloc(int size, int is_index_alloc)
{

}

void r_Free(struct verts_handle_t)
{

}

