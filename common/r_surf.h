#ifndef R_SURF_H
#define R_SURF_H

#include "../gmath/vector.h"


struct texture_params_t
{
    unsigned short target;

    unsigned short width;
    unsigned short height;
    unsigned short depth;

    unsigned short format;
    unsigned short internal_format;
    unsigned short type;

    unsigned short min_filter;
    unsigned short mag_filter;

    unsigned short wrap_s;
    unsigned short wrap_t;
    unsigned short wrap_r;

    unsigned short base_level;
    unsigned short max_level;
};

struct texture_t
{
    unsigned int gl_handle;
    unsigned short texture_params;

    char *name;
};


struct texture_handle_t
{
    unsigned short texture_index;
};

struct load_texture_pixels_params_t
{
    struct texture_handle_t texture_handle;
    char *file_name;
};

struct upload_texture_pixels_params_t
{
    struct texture_handle_t texture_handle;
    struct texture_params_t texture_params;
    unsigned char *texture_data;
    char *file_name;
};


#define R_INVALID_TEXTURE_INDEX 0xffff
#define R_TEXTURE_HANDLE(index)(struct texture_handle_t){index}
#define R_INVALID_TEXTURE_HANDLE R_TEXTURE_HANDLE(R_INVALID_TEXTURE_INDEX)





enum R_MATERIAL_FLAGS
{
    R_MATERIAL_FLAG_INVALID = 1,
};

struct material_t
{
    unsigned char color[4];
    struct texture_handle_t diffuse_texture;
    struct texture_handle_t normal_texture;
    unsigned int flags;
    char *name;
};

#define R_INVALID_MATERIAL_HANDLE -1
#define R_DEFAULT_MATERIAL_HANDLE 0




void r_SetMaterialColor(short material_handle, vec4_t color);

void r_SetMaterialColorPointer(struct material_t *material, vec4_t color);

short r_CreateEmptyMaterial();

short r_CreateMaterial(vec4_t color, struct texture_handle_t diffuse_tex, struct texture_handle_t normal_tex, char *name);

void r_DestroyMaterial(short material_handle);

short r_GetMaterialHandle(char *material_name);

struct material_t *r_GetMaterialPointer(short material_handle);

void r_SetMaterial(int material_handle);





struct texture_handle_t r_CreateEmptyTexture();

/* used by the backend for assynchronous loads... */
int r_LoadTexturePixels(void *params);

/* used by the backend for assynchronous loads... */
void r_UploadTexturePixels(struct upload_texture_pixels_params_t *params);

struct texture_handle_t r_LoadTexture(char *file_name);

struct texture_t *r_GetTexturePointer(struct texture_handle_t texture_handle);

struct texture_params_t *r_GetTextureParamsPointer(struct texture_handle_t texture_handle);


#endif // R_SURF_H






