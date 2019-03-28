#ifndef R_VERTS_H
#define R_VERTS_H





struct verts_handle_t
{
    unsigned index : 31;
    unsigned is_index_alloc : 1;
};

#define R_INVALID_ALLOC_INDEX 0x7fffffff
#define R_INVALID_VERTS_HANDLE (struct verts_handle_t){R_INVALID_ALLOC_INDEX, 1};


void r_InitVerts();

void r_ShutdownVerts();

struct verts_handle_t r_AllocVerts(unsigned int size);

struct verts_handle_t r_AllocIndex(unsigned int size);

struct verts_handle_t r_Alloc(unsigned int size, int is_index_alloc);

void r_Free(struct verts_handle_t);

void r_DefragVerts(int index_list);

void *r_MapAlloc(struct verts_handle_t handle);

void r_UnmapAlloc(struct verts_handle_t handle);

void r_UnmapAllAllocs();

void r_MemcpyTo(struct verts_handle_t dst, void *src, unsigned int size);

void r_MemcpyFrom(void *dst, struct verts_handle_t src, unsigned int size);




#endif // R_VERTS_H
