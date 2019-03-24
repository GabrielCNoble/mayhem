#ifndef S_AUX
#define S_AUX

#include <stdio.h>
#include "vector.h"
#include "list.h"

struct geometry_data_t
{
    struct list_t vertices;
    struct list_t normals;
    struct list_t tangents;
    struct list_t tex_coords;
};

#ifdef __cplusplus
extern "C"
{
#endif

unsigned long aux_FileSize(FILE *file);

void *aux_ReadFile(FILE *file);

void aux_LoadWavefront(char *file_name, struct geometry_data_t *geometry_data);

#ifdef __cplusplus
}
#endif

#endif // S_AUX
