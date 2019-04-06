#ifndef R_LIGHT_H
#define R_LIGHT_H


#include "r_common.h"
#include "vector.h"

#define R_MIN_LIGHT_RADIUS 0.01

struct light_t
{
    vec4_t pos_radius;
    vec4_t col_energy;
};

struct light_interface_t
{
    struct light_t lights[64];
    int light_count;
};


void r_InitLights();

void r_ShutdownLights();

int r_CreateLight(vec3_t position, vec3_t color, float energy, float radius);

void r_DestroyLight(int light_handle);

struct light_t *r_GetLightPointer(int light_handle);

void r_UploadLights(struct light_buffer_t *light_buffer);

void r_EnableLightWrites();




#endif // R_LIGHT_H
