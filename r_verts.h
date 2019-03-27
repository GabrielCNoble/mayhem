#ifndef R_VERTS_H
#define R_VERTS_H





struct verts_handle_t
{
    unsigned index : 31;
    unsigned is_index_alloc : 1;
};



struct verts_handle_t r_AllocVert(int size);

struct verts_handle_t r_AllocIndex(int size);

struct verts_handle_t r_Alloc(int size, int is_index_alloc);

void r_Free(struct verts_handle_t);




#endif // R_VERTS_H
