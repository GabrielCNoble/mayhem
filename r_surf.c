#include "r_surf.h"
#include "stack_list.h"
#include "list.h"

#include "r_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct stack_list_t r_materials(sizeof(struct material_t), 32);

struct stack_list_t r_textures(sizeof(struct texture_t), 32);
struct list_t r_texture_names(sizeof(struct texture_name_t), 32);



unsigned short r_CreateMaterial(vec4_t color, unsigned short diffuse_tex, unsigned short normal_tex, char *name)
{
    unsigned short index;
    int i;
    struct material_t *material;

    index = (unsigned short) r_materials.add(NULL);
    material = (struct material_t *)r_materials.get(index);

    for(i = 0; i < 4; i++)
    {
        if(color[i] < 0.0)
        {
            color[i] = 0.0;
        }
        else if(color[i] > 1.0)
        {
            color[i] = 1.0;
        }

        material->color[i] = 0xff * color[i];
    }

    material->diffuse_texture = diffuse_tex;
    material->normal_texture = normal_tex;
    material->name = strdup(name);

    return index;
}


void r_DestroyMaterial(unsigned short material_handle)
{

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





