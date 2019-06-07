#ifndef R_SURF_H
#define R_SURF_H

#include "../gmath/vector.h"


enum R_MATERIAL_FLAGS
{
    R_MATERIAL_FLAG_INVALID = 1,
};

struct material_t
{
    unsigned char color[4];
    short diffuse_texture;
    short normal_texture;
    unsigned int flags;
    char *name;
};

#define R_INVALID_MATERIAL_HANDLE -1
#define R_DEFAULT_MATERIAL_HANDLE 0




struct texture_t
{
    unsigned int gl_handle;
    unsigned short target;

    unsigned short width;
    unsigned short height;
    unsigned short depth;

    unsigned short format;
    unsigned short internal_format;
    unsigned short type;
};

struct texture_name_t
{
    char *texture_name;
    unsigned short texture_handle;
};

#define R_INVALID_TEXTURE_HANDLE -1


void r_SetMaterialColor(short material_handle, vec4_t color);

void r_SetMaterialColorPointer(struct material_t *material, vec4_t color);

short r_CreateEmptyMaterial();

short r_CreateMaterial(vec4_t color, short diffuse_tex, short normal_tex, char *name);

void r_DestroyMaterial(short material_handle);

short r_GetMaterialHandle(char *material_name);

struct material_t *r_GetMaterialPointer(short material_handle);

void r_SetMaterial(int material_handle);





unsigned int r_CreateGLTexture(unsigned short target, unsigned short width, unsigned short height, unsigned short depth, unsigned short internal_format);

unsigned short r_CreateTexture(unsigned short target, unsigned char *name);

#endif // R_SURF_H






