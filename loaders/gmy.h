#ifndef GMY_H
#define GMY_H

#include "../exchange/xchg.h"
#include "../containers/list.h"
#include "../gmath/vector.h"

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
    char material[64];
    char diffuse_texture[64];
    char normal_texture[64];
    vec4_t base;

    unsigned int vertice_count;
};



void gmy_LoadGmy(char *file_name, struct geometry_data_t *geometry_data);

void gmy_SaveGmy(char *file_name, struct geometry_data_t *geometry_data);


#endif // GMY_H
