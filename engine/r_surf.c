#include "r_surf.h"
#include "r_shader.h"
#include "r_main.h"
//#include "be_backend.h"
#include "res.h"
#include "../common/containers/stack_list.h"
#include "../common/containers/list.h"
#include "../common/utils/file.h"
#include "stb_image_aug.h"

#include "r_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct stack_list_t r_materials(sizeof(struct material_t), 32);
struct stack_list_t r_textures(sizeof(struct texture_t), 32);
struct stack_list_t r_texture_params(sizeof(struct texture_params_t), 32);

extern struct renderer_t r_renderer;

#ifdef __cplusplus
extern "C"
{
#endif

void r_SetMaterialColor(short material, vec4_t color)
{
    if(material != R_DEFAULT_MATERIAL_HANDLE)
    {
        r_SetMaterialColorPointer(r_GetMaterialPointer(material), color);
    }
}

void r_SetMaterialColorPointer(struct material_t *material, vec4_t color)
{
    int i;

    if(material)
    {
        if(material != r_GetMaterialPointer(R_DEFAULT_MATERIAL_HANDLE))
        {
            for(i = 0; i < 4; i++)
            {
                if(color[i] > 1.0) color[i] = 1.0;
                else if(color[i] < 0.0) color[i] = 0.0;
                material->color[i] = 0xff * color[i];
            }
        }
    }
}

/*
    primarily to be used by loaders...
*/
short r_CreateEmptyMaterial()
{
    short material_handle;
    struct material_t *material;

    material_handle = r_materials.add(NULL);
    material = (struct material_t *)r_materials.get(material_handle);

    material->flags = 0;

    return material_handle;
}

short r_CreateMaterial(vec4_t color, struct texture_handle_t diffuse_tex, struct texture_handle_t normal_tex, char *name)
{
    short material_handle = R_DEFAULT_MATERIAL_HANDLE;
    struct material_t *material;

    if(strcmp(name, "default_material") || (!r_materials.cursor))
    {
        material_handle = r_CreateEmptyMaterial();
        material = r_GetMaterialPointer(material_handle);

        r_SetMaterialColorPointer(material, color);

        material->diffuse_texture = diffuse_tex;
        material->normal_texture = normal_tex;
        material->name = strdup(name);
    }

    return material_handle;
}

void r_DestroyMaterial(short material_handle)
{
    struct material_t *material;

    if(material_handle != R_DEFAULT_MATERIAL_HANDLE)
    {
        material = r_GetMaterialPointer(material_handle);

        material->flags |= R_MATERIAL_FLAG_INVALID;

        r_materials.remove(material_handle);
    }
}

short r_GetMaterialHandle(char *material_name)
{
    int i;
    struct material_t *materials;

    materials = (struct material_t *)r_materials.buffer;

    for(i = 0; i < r_materials.cursor; i++)
    {
        if(materials[i].flags & R_MATERIAL_FLAG_INVALID)
        {
            continue;
        }

        if(!strcmp(material_name, materials[i].name))
        {
            return i;
        }
    }

    return R_DEFAULT_MATERIAL_HANDLE;
}

struct material_t *r_GetMaterialPointer(short material_handle)
{
    struct material_t *material;

    material = (struct material_t *)r_materials.get(material_handle);

    if(material)
    {
        if(material->flags & R_MATERIAL_FLAG_INVALID)
        {
            material = NULL;
        }
    }

    return material;
}

void r_SetMaterial(int material_handle)
{
    struct material_t *material;
    struct texture_t *texture;
    vec4_t base_color;

    int material_flags = 0;

    material = r_GetMaterialPointer(material_handle);

    if(material)
    {
        r_renderer.active_material = material_handle;

        base_color.x = (float)material->color[0] / (float)0xff;
        base_color.y = (float)material->color[1] / (float)0xff;
        base_color.z = (float)material->color[2] / (float)0xff;
        base_color.w = (float)material->color[3] / (float)0xff;

        if(material->diffuse_texture.texture_index != R_INVALID_TEXTURE_INDEX)
        {
            texture = r_GetTexturePointer(material->diffuse_texture);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture->gl_handle);
            r_DefaultUniform1i(r_TextureSampler0, 0);
            material_flags |= 1;
        }

        if(material->normal_texture.texture_index != R_INVALID_TEXTURE_INDEX)
        {
            texture = r_GetTexturePointer(material->normal_texture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture->gl_handle);
            r_DefaultUniform1i(r_TextureSampler1, 1);
            material_flags |= 2;
        }


        r_DefaultUniform1i(r_MaterialFlags, material_flags);
        r_DefaultUniform4fv(r_BaseColor, (float *) base_color.floats);
    }
}

unsigned int r_CreateGLTexture(unsigned short target,
                               unsigned short width,
                               unsigned short height,
                               unsigned short depth,
                               unsigned short internal_format,
                               unsigned short min_filter,
                               unsigned short mag_filter,
                               unsigned short wrap_s,
                               unsigned short wrap_t,
                               unsigned short wrap_r,
                               unsigned short base_level,
                               unsigned short max_level)
{
    unsigned int texture;
    unsigned short format;
    unsigned short type;

    switch(target)
    {
        case GL_TEXTURE_2D:
        case GL_TEXTURE_CUBE_MAP:
        case GL_TEXTURE_2D_ARRAY:
        break;

        default:
            return 0;
    }

    switch(internal_format)
    {
        case GL_R32F:
            format = GL_RED;
        break;

        case GL_RGBA8:
            format = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
        break;

        case GL_RGBA16F:
            format = GL_RGBA;
            type = GL_FLOAT;
        break;

        case GL_RGBA32F:
            format = GL_RGBA;
            type = GL_FLOAT;
        break;

        case GL_RGB32F:
            format = GL_RGB;
            type = GL_FLOAT;
        break;

        case GL_RGB8:
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
        break;

        default:
            return 0;
    }

    while(glGetError() != GL_NO_ERROR);

    glGenTextures(1, &texture);
    glBindTexture(target, texture);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag_filter);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, base_level);
    glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, max_level);
    glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY, 8.0);

    if(target == GL_TEXTURE_3D)
    {
        glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap_r);
    }



    return texture;
}

struct texture_handle_t r_CreateEmptyTexture()
{
    struct texture_handle_t texture_handle = R_INVALID_TEXTURE_HANDLE;
    struct texture_t *texture;

    texture_handle.texture_index = r_textures.add(NULL);
    texture = (struct texture_t *)r_textures.get(texture_handle.texture_index);

    memset(texture, 0, sizeof(struct texture_t));

    texture->texture_params = r_texture_params.add(NULL);

    return texture_handle;
}

/* used by the backend for assynchronous loads... */
int r_LoadTexturePixels(void *params)
{
    struct load_texture_pixels_params_t local_params;
    struct upload_texture_pixels_params_t upload_pixels_params;
    int texture_components;

    int width;
    int height;

    memcpy(&local_params, params, sizeof(struct load_texture_pixels_params_t));
//    local_params = *params;

    upload_pixels_params.texture_data = stbi_load(local_params.file_name,
                            &width,
                            &height,
                            &texture_components, STBI_rgb_alpha);

    upload_pixels_params.texture_params.width = width;
    upload_pixels_params.texture_params.height = height;
    upload_pixels_params.texture_handle = local_params.texture_handle;
    upload_pixels_params.file_name = local_params.file_name;

    r_QueueCmd(R_CMD_UPLOAD_TEXTURE, &upload_pixels_params, sizeof(struct upload_texture_pixels_params_t));


    printf("texture [%s] decompressed!\n", local_params.file_name);

    return 0;
}

/* used by the backend for assynchronous loads... */
void r_UploadTexturePixels(struct upload_texture_pixels_params_t *params)
{
    struct texture_t *texture = r_GetTexturePointer(params->texture_handle);
    struct texture_params_t *texture_params;

    if(texture)
    {
        texture_params = r_GetTextureParamsPointer(params->texture_handle);

        texture_params->width = params->texture_params.width;
        texture_params->height = params->texture_params.height;
        texture_params->depth = 0;

        texture_params->internal_format = GL_RGBA8;
        texture_params->format = GL_RGBA;
        texture_params->type = GL_UNSIGNED_BYTE;
        texture_params->target = GL_TEXTURE_2D;
        texture_params->wrap_s = GL_REPEAT;
        texture_params->wrap_t = GL_REPEAT;
        texture_params->wrap_r = 0;
        texture_params->base_level = 0;
        texture_params->max_level = 4;
        texture_params->min_filter = GL_LINEAR_MIPMAP_LINEAR;
        texture_params->mag_filter = GL_LINEAR_MIPMAP_LINEAR;

        texture->gl_handle = r_CreateGLTexture(texture_params->target,
                                               texture_params->width,
                                               texture_params->height,
                                               0,
                                               texture_params->internal_format,
                                               texture_params->min_filter,
                                               texture_params->mag_filter,
                                               texture_params->wrap_s,
                                               texture_params->wrap_t,
                                               texture_params->wrap_r,
                                               texture_params->base_level,
                                               texture_params->max_level);

        glBindTexture(GL_TEXTURE_2D, texture->gl_handle);
        glTexImage2D(GL_TEXTURE_2D, 0, texture_params->internal_format, texture_params->width, texture_params->height, 0, texture_params->format, texture_params->type, params->texture_data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        printf("texture [%s] uploaded!\n", params->file_name);
    }
}

struct texture_handle_t r_LoadTexture(char *file_name)
{
    struct texture_handle_t texture_handle = R_INVALID_TEXTURE_HANDLE;
    struct load_texture_pixels_params_t load_texture_params;
    struct texture_t *texture;

    if(file_FileExists(file_name))
    {
        printf("loading texture [%s]...\n", file_name);

        texture_handle = r_CreateEmptyTexture();
        texture = r_GetTexturePointer(texture_handle);

        texture->name = strdup(file_name);

        load_texture_params.file_name = texture->name;
        load_texture_params.texture_handle = texture_handle;

        res_QueueCmd(r_LoadTexturePixels, &load_texture_params, sizeof(struct load_texture_pixels_params_t));
//        be_QueueCmd(BE_CMD_LOAD_TEXTURE, &load_texture_params, sizeof(struct load_texture_pixels_params_t));
    }

    return texture_handle;
//
//    int texture_width;
//    int texture_height;
//    int texture_components;
//    unsigned char *texture_data;
//
//    texture_data = stbi_load(file_name, &texture_width, &texture_height, &texture_components, STBI_rgb_alpha);
//
//    if(texture_data)
//    {
//        texture_handle = r_CreateEmptyTexture();
//        texture = r_GetTexturePointer(texture_handle);
//
//        texture->gl_handle = r_CreateGLTexture(GL_TEXTURE_2D, texture_width, texture_height, 0,
//                                               GL_RGBA8, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR,
//                                               GL_REPEAT, GL_REPEAT, 0, 0, 4);
//
//
//        texture_params = r_GetTextureParamsPointer(texture_handle);
//
//        texture_params->width = texture_width;
//        texture_params->height = texture_height;
//        texture_params->depth = 0;
//
//        texture_params->internal_format = GL_RGBA8;
//        texture_params->format = GL_RGBA;
//        texture_params->type = GL_UNSIGNED_BYTE;
//        texture_params->target = GL_TEXTURE_2D;
//        texture_params->wrap_s = GL_REPEAT;
//        texture_params->wrap_t = GL_REPEAT;
//        texture_params->wrap_r = 0;
//        texture_params->base_level = 0;
//        texture_params->max_level = 4;
//        texture_params->min_filter = GL_LINEAR_MIPMAP_LINEAR;
//        texture_params->mag_filter = GL_LINEAR_MIPMAP_LINEAR;
//
//        glBindTexture(GL_TEXTURE_2D, texture->gl_handle);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//        glBindTexture(GL_TEXTURE_2D, 0);
//
//        stbi_image_free(texture_data);
//    }
//
//    return texture_handle;
}

struct texture_t *r_GetTexturePointer(struct texture_handle_t texture_handle)
{
    struct texture_t *texture = NULL;

    if(texture_handle.texture_index != R_INVALID_TEXTURE_INDEX)
    {
        texture = (struct texture_t *)r_textures.get(texture_handle.texture_index);

//        if(!texture->name)
//        {
//            texture = NULL;
//        }
    }

    return texture;
}



struct texture_params_t *r_GetTextureParamsPointer(struct texture_handle_t texture_handle)
{
    struct texture_t *texture;
    struct texture_params_t *texture_params = NULL;

    texture = r_GetTexturePointer(texture_handle);

    if(texture)
    {
        texture_params = (struct texture_params_t *)r_texture_params.get(texture->texture_params);
    }

    return texture_params;
}

#ifdef __cplusplus
}
#endif





