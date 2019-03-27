#ifndef R_SURF_H
#define R_SURF_H

#include "vector.h"

struct material_t
{
    unsigned char color[4];
    unsigned short diffuse_texture;
    unsigned short normal_texture;

    char *name;
};

#define INVALID_MATERIAL_HANDLE 0xffff




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

#define INVALID_TEXTURE_HANDLE 0xffff




unsigned short r_CreateMaterial(vec4_t color, unsigned short diffuse_tex, unsigned short normal_tex, char *name);

void r_DestroyMaterial(unsigned short material_handle);

unsigned int r_CreateGLTexture(unsigned short target, unsigned short width, unsigned short height, unsigned short depth, unsigned short internal_format);

unsigned short r_CreateTexture(unsigned short target, unsigned char *name);

#endif // R_SURF_H






