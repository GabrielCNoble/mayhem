#include "r_shader.h"
#include "r_common.h"
#include "stack_list.h"
#include "s_aux.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct stack_list_t r_shaders(sizeof(struct shader_t), 32);


int r_CreateShader(char *name)
{
    int shader_handle;
    struct shader_t *shader;

    shader_handle = r_shaders.add(NULL);
    shader = (struct shader_t *)r_shaders.get(shader_handle);

    shader->name = strdup(name);

    shader->program = glCreateProgram();
    shader->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    shader->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    return shader_handle;
}

void r_DestroyShader(int shader_handle)
{
    struct shader_t *shader;

    shader = r_GetShaderPointer(shader_handle);

    if(shader)
    {
        glDeleteProgram(shader->program);
        glDeleteShader(shader->vertex_shader);
        glDeleteShader(shader->fragment_shader);
        free(shader->name);
        shader->program = 0;
        r_shaders.remove(shader_handle);
    }
}

char *r_LoadShaderSource(char *file_name)
{
    FILE *vertex_shader;
    FILE *fragment_shader;

    unsigned long vertex_source_len;
    unsigned long fragment_source_len;

    char *vertex_source;
    char *fragment_source;

    char *source = NULL;

    char full_name[128];

    strcpy(full_name, file_name);
    strcat(full_name, ".vert");
    vertex_shader = fopen(full_name, "rb");

    strcpy(full_name, file_name);
    strcat(full_name, ".frag");
    fragment_shader = fopen(full_name, "rb");


    if(vertex_shader && fragment_shader)
    {
        vertex_source_len = aux_FileSize(vertex_shader);
        fragment_source_len = aux_FileSize(fragment_shader);
        source = (char *)calloc(1, vertex_source_len + fragment_source_len + 8);

        vertex_source = (char *)aux_ReadFile(vertex_shader);
        vertex_source[vertex_source_len - 1] = '\0';
        /* pre-processing happens here... */


        fragment_source = (char *)aux_ReadFile(fragment_shader);
        fragment_source[fragment_source_len - 1] = '\0';
        /* pre-processing happens here... */


        strcpy(source, vertex_source);
        strcpy(source + vertex_source_len + 1, fragment_source);

        free(vertex_source);
        free(fragment_source);
    }

    if(vertex_shader)
    {
        fclose(vertex_shader);
    }

    if(fragment_shader)
    {
        fclose(fragment_shader);
    }

    return source;
}

int r_CompileShaderSource(int shader_handle, char *source)
{
    struct shader_t *shader;
    int compilation_status;

    char *vertex_source;
    char *fragment_source;

    shader = r_GetShaderPointer(shader_handle);

    if(!(shader && source))
    {
        return 0;
    }

    vertex_source = source;
    fragment_source = source + strlen(source) + 2;

    glShaderSource(shader->vertex_shader, 1, &vertex_source, NULL);
    glCompileShader(shader->vertex_shader);
    glGetShaderiv(shader->vertex_shader, GL_COMPILE_STATUS, &compilation_status);

    if(!compilation_status)
    {
        printf("vertex shader deu ruim...\n");
        return 0;
    }

    glShaderSource(shader->fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(shader->fragment_shader);
    glGetShaderiv(shader->fragment_shader, GL_COMPILE_STATUS, &compilation_status);

    if(!compilation_status)
    {
        printf("fragment shader deu ruim...\n");
        return 0;
    }

    glAttachShader(shader->program, shader->vertex_shader);
    glAttachShader(shader->program, shader->fragment_shader);
    glLinkProgram(shader->program);
    glGetProgramiv(shader->program, GL_LINK_STATUS, &compilation_status);

    if(!compilation_status)
    {
        printf("linkagem deu ruim...\n");
        return 0;
    }


    return 1;
}

int r_LoadShader(char *file_name)
{
    int shader_handle;
    char *shader_source;

    shader_handle = r_CreateShader(file_name);
    shader_source = r_LoadShaderSource(file_name);

    if(shader_source)
    {
        r_CompileShaderSource(shader_handle, shader_source);
    }
}

struct shader_t *r_GetShaderPointer(int shader_handle)
{
    struct shader_t *shader = NULL;

    shader = (struct shader_t *)r_shaders.get(shader_handle);

    if(shader)
    {
        if(!shader->program)
        {
            shader = NULL;
        }
    }

    return shader;
}
