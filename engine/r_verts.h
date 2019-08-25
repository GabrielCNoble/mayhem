#ifndef R_VERTS_H
#define R_VERTS_H


#include "../common/gmath/vector.h"


struct verts_handle_t
{
    unsigned index : 31;
    unsigned is_index_alloc : 1;
};

#define R_INVALID_ALLOC_INDEX 0x7fffffff
#define R_INVALID_VERTS_HANDLE (struct verts_handle_t){R_INVALID_ALLOC_INDEX, 1}

struct vertex_t
{
    vec3_t position;
    vec3_t normal;
    vec3_t tangent;
    vec2_t tex_coord;
};


void r_InitVerts();

void r_ShutdownVerts();

struct verts_handle_t r_AllocVerts(unsigned int size, int alignment);

struct verts_handle_t r_AllocIndex(unsigned int size, int alignment);

struct verts_handle_t r_Alloc(unsigned int size, int alignment, int is_index_alloc);

void r_Free(struct verts_handle_t handle);

int r_GetAllocStart(struct verts_handle_t handle);

void r_DefragVerts(int index_list);

void *r_MapAlloc(struct verts_handle_t handle);

void r_UnmapAlloc(struct verts_handle_t handle);

void r_UnmapAllAllocs();

void r_MemcpyTo(struct verts_handle_t dst, void *src, unsigned int size);

void r_MemcpyToUnmapped(struct verts_handle_t dst, void *src, unsigned int size);

void r_MemcpyFrom(void *dst, struct verts_handle_t src, unsigned int size);

void r_EnableVertsReads();

void r_DisableVertsReads();




#endif // R_VERTS_H
