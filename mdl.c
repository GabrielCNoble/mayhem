#include "mdl.h"
#include "stack_list.h"

#include <stdio.h>

struct stack_list_t mdl_models(sizeof(struct model_t), 32);



struct model_handle_t mdl_CreateEmptyModel()
{
    struct model_handle_t handle = INVALID_MODEL_HANDLE;
    struct model_t *model;


    handle.index = mdl_models.add(NULL);
    model = (struct model_t* )mdl_models.get(handle.index);
    model->verts_handle = R_INVALID_VERTS_HANDLE;
    model->invalid = 0;

    return handle;
}

void mdl_DestroyModel(struct model_handle_t model_handle)
{
    struct model_t *model;

    model = mdl_GetModelPointer(model_handle);

    if(model)
    {
        if(model->verts_handle.index != R_INVALID_ALLOC_INDEX)
        {
            r_Free(model->verts_handle);
        }

        model->invalid = 1;
        mdl_models.remove(model_handle.index);
    }
}

struct model_t *mdl_GetModelPointer(struct model_handle_t model_handle)
{
    struct model_t *model = NULL;

    if(model_handle.index < INVALID_MODEL_INDEX)
    {
        model = (struct model_t* )mdl_models.get(model_handle.index);

        if(model->invalid)
        {
            model = NULL;
        }
    }

    return model;
}



