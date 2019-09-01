#include "mdl.h"
#include "r_common.h"
#include "r_surf.h"
#include "r_main.h"
//#include "be_backend.h"
#include "../common/containers/stack_list.h"
#include "../loaders/gmy.h"

#include <stdio.h>
#include <stdlib.h>

struct stack_list_t mdl_models(sizeof(struct model_t), 32);

#ifdef __cplusplus
extern "C"
{
#endif


__declspec(dllexport) struct model_handle_t mdl_CreateEmptyModel()
{
    struct model_handle_t handle = INVALID_MODEL_HANDLE;
    struct model_t *model;

    handle.index = mdl_models.add(NULL);
    model = (struct model_t* )mdl_models.get(handle.index);
    model->verts_handle = R_INVALID_VERTS_HANDLE;
    model->invalid = 0;

    return handle;
}

__declspec(dllexport) void mdl_DestroyModel(struct model_handle_t model_handle)
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

__declspec(dllexport) struct model_t *mdl_GetModelPointer(struct model_handle_t model_handle)
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

__declspec(dllexport) struct model_handle_t mdl_LoadModel(char *file_name)
{
    struct geometry_data_t geometry_data;
    struct model_handle_t model_handle;
    struct model_t *model;
    struct vertex_t *verts;

    vec3_t *vertices;
    vec3_t *normals;
    vec3_t *tangents;
    vec2_t *tex_coords;
    struct batch_data_t *batches;
    struct draw_batch_t batch;
    int model_start;
    int i;

    gmy_LoadGmy(file_name, &geometry_data);

    model_handle = mdl_CreateEmptyModel();
    model = mdl_GetModelPointer(model_handle);

    model->batches.init(sizeof(struct draw_batch_t), geometry_data.batches.cursor);
    batches = (struct batch_data_t *)geometry_data.batches.buffer;
//    model->batches = geometry_data.batches;
    model->verts_handle = r_AllocVerts(sizeof(struct vertex_t) * geometry_data.vertices.cursor, sizeof(vertex_t));
    verts = (struct vertex_t *)calloc(sizeof(struct vertex_t), geometry_data.vertices.cursor);
    model_start = r_GetAllocStart(model->verts_handle) / sizeof(struct vertex_t);

    for(i = 0; i < geometry_data.batches.cursor; i++)
    {
        batch.material = r_CreateMaterial(batches[i].base_color, R_INVALID_TEXTURE_HANDLE, R_INVALID_TEXTURE_HANDLE, batches[i].material);
        batch.start = batches[i].start + model_start;
        batch.count = batches[i].count;
        model->batches.add(&batch);
    }

    vertices = (vec3_t *)geometry_data.vertices.buffer;
    normals = (vec3_t *)geometry_data.normals.buffer;
    tangents = (vec3_t *)geometry_data.tangents.buffer;
    tex_coords = (vec2_t *)geometry_data.tex_coords.buffer;

    for(i = 0; i < geometry_data.vertices.cursor; i++)
    {
        verts[i].position = vertices[i];
        verts[i].normal = normals[i];
        verts[i].tangent = tangents[i];
        verts[i].tex_coord = tex_coords[i];
    }

    be_MemcpyTo(model->verts_handle, verts, sizeof(vertex_t) * geometry_data.vertices.cursor);
    free(verts);

    return model_handle;
}

#ifdef __cplusplus
}
#endif





