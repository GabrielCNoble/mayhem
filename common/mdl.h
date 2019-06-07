#ifndef MDL_H
#define MDL_H


#include "r_verts.h"


struct model_handle_t
{
    unsigned int index;
};

#define MODEL_HANDLE(index) (struct model_handle_t){index};
#define INVALID_MODEL_INDEX 0xffffffff
#define INVALID_MODEL_HANDLE MODEL_HANDLE(INVALID_MODEL_INDEX)

struct model_t
{
    struct verts_handle_t verts_handle;
    int invalid;
};





struct model_handle_t mdl_CreateEmptyModel();

void mdl_DestroyModel(struct model_handle_t model_handle);

struct model_t *mdl_GetModelPointer(struct model_handle_t model_handle);


#endif // MDL_H
