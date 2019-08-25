#ifndef OBJ_H
#define OBJ_H


#include "../common/exchange/xchg.h"

void obj_LoadWavefront(char *file_name, struct geometry_data_t *geometry_data);

void obj_LoadWavefrontMtl(char *file_name, struct geometry_data_t *geometry_data);

struct batch_data_t *obj_GetBatch(char *material_name, struct geometry_data_t *geometry_data);


#endif // OBJ_H
