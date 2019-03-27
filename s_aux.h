#ifndef S_AUX
#define S_AUX

#include <stdio.h>
#include "vector.h"
#include "list.h"
#include "r_common.h"
#include "r_surf.h"

struct geometry_material_t
{
    vec4_t color;
    char *name;
    char *diffuse_texture;
    char *normal_texture;
};

struct geometry_data_t
{
    struct list_t vertices;
    struct list_t normals;
    struct list_t tangents;
    struct list_t tex_coords;

    struct list_t materials;
    struct list_t draw_batches;
};

#ifdef __cplusplus
extern "C"
{
#endif

unsigned long aux_FileSize(FILE *file);

void *aux_ReadFile(FILE *file);

void aux_LoadWavefront(char *file_name, struct geometry_data_t *geometry_data);

void aux_LoadWavefrontMtl(char *file_name, struct geometry_data_t *geometry_data);

#ifdef __cplusplus
}
#endif

#endif // S_AUX
