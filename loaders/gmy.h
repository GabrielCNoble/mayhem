#ifndef GMY_H
#define GMY_H

#include "../common/exchange/xchg.h"
#include "../common/containers/list.h"
#include "../common/gmath/vector.h"

static char gmy_start_tag[] = "[gmy_start]";
struct gmy_start_t
{
    char tag[(sizeof(gmy_start_tag) + 3) & (~3)];
    unsigned int batch_count;
    unsigned int vertice_count;
};


static char gmy_end_tag[] = "[gmy_end]";
struct gmy_end_t
{
    char tag[(sizeof(gmy_end_tag) + 3) & (~3)];
};


static char gmy_batch_tag[] = "[gmy_batch]";
struct gmy_batch_t
{
    char tag[(sizeof(gmy_batch_tag) + 3) & (~3)];
    struct batch_data_t batch;
//    char material[64];
//    char diffuse_texture[64];
//    char normal_texture[64];
//    vec4_t base;
//
//    unsigned int vertice_count;
};


void gmy_LoadGmy(char *file_name, struct geometry_data_t *geometry_data);

void gmy_DeserializeGmy(void **buffer, struct geometry_data_t *geometry_data, int advance_pointer);

void gmy_SaveGmy(char *file_name, struct geometry_data_t *geometry_data);

void gmy_SerializeGmy(void **buffer, unsigned int *buffer_size, struct geometry_data_t *geometry_data);


#endif // GMY_H




