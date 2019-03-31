#include "r_surf.h"
#include "r_shader.h"
#include "stack_list.h"
#include "list.h"

#include "r_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct stack_list_t r_materials(sizeof(struct material_t), 32);

struct stack_list_t r_textures(sizeof(struct texture_t), 32);
struct list_t r_texture_names(sizeof(struct texture_name_t), 32);


extern struct renderer_t r_renderer;

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

short r_CreateMaterial(vec4_t color, short diffuse_tex, short normal_tex, char *name)
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
    vec4_t base_color;

    material = r_GetMaterialPointer(material_handle);

    if(material)
    {
        r_renderer.active_material = material_handle;

        base_color.x = (float)material->color[0] / (float)0xff;
        base_color.y = (float)material->color[1] / (float)0xff;
        base_color.z = (float)material->color[2] / (float)0xff;
        base_color.w = (float)material->color[3] / (float)0xff;

        r_DefaultUniform4fv(r_BaseColor, (float *) base_color.floats);
    }
}

unsigned int r_CreateGLTexture(unsigned short target, unsigned short width, unsigned short height, unsigned short depth, unsigned short internal_format)
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

        case GL_RGB8:
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
        break;

        default:
            return 0;
    }
}


unsigned short r_CreateTexture(unsigned short target, char *name)
{
//    struct texture_t *texture;
//    struct texture_name_t texture_name;
//    unsigned short texture_handle;
//    unsigned short target;
//
//
//    switch(target)
//    {
//        case GL_TEXTURE_2D:
//        case GL_TEXTURE_CUBE_MAP:
//        case GL_TEXTURE_2D_ARRAY:
//        break;
//
//        default:
//            return INVALID_TEXTURE_HANDLE;
//    }
//
//    texture_handle = (unsigned short)r_textures.add(NULL);
//    texture = (struct texture_t *)r_textures.get(texture_handle);
//
//    glGenTextures(1, &texture->gl_handle);
//    texture->target = target;
}





