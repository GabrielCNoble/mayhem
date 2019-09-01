#include "r_shader.h"
#include "r_common.h"
#include "r_verts.h"
#include "../common/containers/stack_list.h"
#include "../common/utils/file.h"
//#include "s_aux.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct stack_list_t r_shaders(sizeof(struct shader_t), 32);

extern struct renderer_t r_renderer;

char *r_default_uniform_names[r_LAST_UNIFORM];

#ifdef __cplusplus
extern "C"
{
#endif

void r_InitShader()
{
    #define STRFY(x) #x
    r_default_uniform_names[r_ModelViewProjectionMatrix] = STRFY(r_ModelViewProjectionMatrix);
    r_default_uniform_names[r_ViewProjectionMatrix] = STRFY(r_ViewProjectionMatrix);
    r_default_uniform_names[r_ViewMatrix] = STRFY(r_ViewMatrix);
    r_default_uniform_names[r_ViewModelMatrix] = STRFY(r_ViewModelMatrix);
    r_default_uniform_names[r_BaseColor] = STRFY(r_BaseColor);
    r_default_uniform_names[r_DebugColor] = STRFY(r_DebugColor);
    r_default_uniform_names[r_NearPlane] = STRFY(r_NearPlane);
    r_default_uniform_names[r_MaterialFlags] = STRFY(r_MaterialFlags);
    //r_default_uniform_names[r_LightCount] = STRFY(r_LightCount);
    r_default_uniform_names[r_TextureSampler0] = STRFY(r_TextureSampler0);
    r_default_uniform_names[r_TextureSampler1] = STRFY(r_TextureSampler1);
    #undef STRFY


    r_renderer.lit_shader = r_LoadShader("shaders/lit");
    r_renderer.portal_stencil_shader = r_LoadShader("shaders/portal_stencil");
    r_renderer.clear_portal_depth_shader = r_LoadShader("shaders/clear_portal_depth");
    r_renderer.debug_shader = r_LoadShader("shaders/dbg");
}

void r_ShutdownShader()
{

}

int r_CreateShader(char *name)
{
    int shader_handle;
    struct shader_t *shader;
    int i;

    shader_handle = r_shaders.add(NULL);
    shader = (struct shader_t *)r_shaders.get(shader_handle);

    shader->name = strdup(name);
    shader->default_uniforms = (struct uniform_t *)calloc(sizeof(struct uniform_t), r_LAST_UNIFORM);

    shader->program = glCreateProgram();
    shader->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    shader->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    shader->vertex_position = 0;
    shader->vertex_normal = 0;

    for(i = 0; i < r_LAST_UNIFORM; i++)
    {
        shader->default_uniforms[i].location = -1;
    }

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

    long vertex_source_len;
    long fragment_source_len;

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
        vertex_source_len = file_FileSize(vertex_shader);
        fragment_source_len = file_FileSize(fragment_shader);
        source = (char *)calloc(1, vertex_source_len + fragment_source_len + 8);
//
//        vertex_source = (char *)file_ReadFile(vertex_shader);
//        vertex_source[vertex_source_len - 1] = '\0';
//        /* pre-processing happens here... */
//
//
//        fragment_source = (char *)file_ReadFile(fragment_shader);
//        fragment_source[fragment_source_len - 1] = '\0';
//        /* pre-processing happens here... */

        file_ReadFile(vertex_shader, (void **)&vertex_source, &vertex_source_len);
        vertex_source[vertex_source_len - 1] = '\0';
        file_ReadFile(fragment_shader, (void **)&fragment_source, &fragment_source_len);
        fragment_source[fragment_source_len - 1] = '\0';

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
    int success = 1;
    int compilation_status = 0;
    char *log;
    int log_len;

    unsigned int i;

    char *vertex_source;
    char *fragment_source;

    shader = r_GetShaderPointer(shader_handle);

    if(!(shader && source))
    {
        return 0;
    }

    printf("=============================================\n");
    printf("compiling shader [%s]\n", shader->name);
    printf("=============================================\n");

    log = (char *)malloc(2048);

    vertex_source = source;
    fragment_source = source + strlen(source) + 2;

    glShaderSource(shader->vertex_shader, 1, &vertex_source, NULL);
    glCompileShader(shader->vertex_shader);
    glGetShaderiv(shader->vertex_shader, GL_COMPILE_STATUS, &compilation_status);
    glGetShaderInfoLog(shader->vertex_shader, 2048, &log_len, log);
    printf("vertex stage log: \n%s\n", log);
    success = success && compilation_status;

    glShaderSource(shader->fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(shader->fragment_shader);
    glGetShaderiv(shader->fragment_shader, GL_COMPILE_STATUS, &compilation_status);
    glGetShaderInfoLog(shader->fragment_shader, 2048, &log_len, log);
    printf("fragment stage log: \n%s\n", log);
    success = success && compilation_status;

    glAttachShader(shader->program, shader->vertex_shader);
    glAttachShader(shader->program, shader->fragment_shader);
    glLinkProgram(shader->program);
    glGetProgramiv(shader->program, GL_LINK_STATUS, &compilation_status);
    glGetProgramInfoLog(shader->program, 2048, &log_len, log);
    printf("program log: \n%s\n", log);
    success = success && compilation_status;

    printf("=============================================\n");
    printf("compilation status: %s\n", success ? "ok" : "failed");
    printf("=============================================\n\n");

    free(log);

    if(!success)
    {
        return 0;
    }

    shader->vertex_position = glGetAttribLocation(shader->program, "r_VertexPosition");
    shader->vertex_normal = glGetAttribLocation(shader->program, "r_VertexNormal");
    shader->vertex_tangent = glGetAttribLocation(shader->program, "r_VertexTangent");
    shader->vertex_tex_coords = glGetAttribLocation(shader->program, "r_VertexTexCoords");
    shader->vertex_color = glGetAttribLocation(shader->program, "r_VertexColor");

    for(i = 0; i < r_LAST_UNIFORM; i++)
    {
        shader->default_uniforms[i].location = glGetUniformLocation(shader->program, r_default_uniform_names[i]);
    }

    if((i = glGetUniformBlockIndex(shader->program, "r_LightsUniformBlock")) != GL_INVALID_INDEX)
    {
        glUniformBlockBinding(shader->program, i, R_LIGHTS_UNIFORM_BUFFER_BINDING);
    }

    if((i = glGetUniformBlockIndex(shader->program, "r_TrianglesUniformBlock")) != GL_INVALID_INDEX)
    {
        glUniformBlockBinding(shader->program, i, R_TRIANGLES_UNIFORM_BUFFER_BINDING);
    }

    if((i = glGetUniformBlockIndex(shader->program, "r_TriangleIndicesUniformBlock")) != GL_INVALID_INDEX)
    {
        glUniformBlockBinding(shader->program, i, R_TRIANGLE_INDICES_UNIFORM_BUFFER_BINDING);
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
        free(shader_source);
    }

    return shader_handle;
}

int r_GetShader(char *name)
{
    struct shader_t *shaders;
    int i;

    shaders = (struct shader_t *)r_shaders.buffer;

    for(i = 0; i < r_shaders.cursor; i++)
    {
        if(!strcmp(shaders[i].name, name))
        {
            if(shaders[i].program)
            {
                return i;
            }
        }
    }

    return -1;
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



void r_SetShader(int shader_handle)
{
    struct shader_t *shader;

    if(shader_handle > -1)
    {
        shader = r_GetShaderPointer(shader_handle);

        if(shader)
        {
            r_renderer.active_shader = shader_handle;

            glUseProgram(shader->program);

            if(shader->vertex_position > -1)
            {
                glEnableVertexAttribArray(shader->vertex_position);
                glVertexAttribPointer(shader->vertex_position, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex_t), &(((struct vertex_t *)0)->position));
            }

            if(shader->vertex_normal > -1)
            {
                glEnableVertexAttribArray(shader->vertex_normal);
                glVertexAttribPointer(shader->vertex_normal, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex_t), &(((struct vertex_t *)0)->normal));
            }

            if(shader->vertex_tangent > -1)
            {
                glEnableVertexAttribArray(shader->vertex_tangent);
                glVertexAttribPointer(shader->vertex_tangent, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex_t), &(((struct vertex_t *)0)->tangent));
            }

            if(shader->vertex_tex_coords > -1)
            {
                glEnableVertexAttribArray(shader->vertex_tex_coords);
                glVertexAttribPointer(shader->vertex_tex_coords, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_t), &(((struct vertex_t *)0)->tex_coord));
            }

//            if(shader->vertex_color > -1)
//            {
//                glEnableVertexAttribArray(shader->vertex_color);
//                glVertexAttribPointer(shader->vertex_color, 4, GL_FLOAT, GL_FALSE, sizeof(struct vertex_t), &(((struct vertex_t *)0)->vertex_color));
//            }

        }
    }
    else
    {
        glUseProgram(0);
    }
}


void r_DefaultUniformMatrix4fv(int uniform, float *value)
{
    struct shader_t *shader;

    shader = r_GetShaderPointer(r_renderer.active_shader);

    if(shader)
    {
        r_UniformMatrix4fv(shader->default_uniforms + uniform, value);
    }
}

void r_UniformMatrix4fv(struct uniform_t *uniform, float *value)
{
    if(uniform)
    {
        glUniformMatrix4fv(uniform->location, 1, GL_FALSE, value);
    }
}

void r_DefaultUniform4fv(int uniform, float *value)
{
    struct shader_t *shader;

    shader = r_GetShaderPointer(r_renderer.active_shader);

    if(shader)
    {
        r_Uniform4fv(shader->default_uniforms + uniform, value);
    }
}

void r_DefaultUniform1i(int uniform, int value)
{
    struct shader_t *shader;

    shader = r_GetShaderPointer(r_renderer.active_shader);

    if(shader)
    {
        r_Uniform1i(shader->default_uniforms + uniform, value);
    }
}

void r_Uniform4fv(struct uniform_t *uniform, float *value)
{
    if(uniform)
    {
        glUniform4fv(uniform->location, 1, value);
    }
}

void r_Uniform1i(struct uniform_t *uniform, int value)
{
    if(uniform)
    {
        glUniform1i(uniform->location, value);
    }
}


unsigned int r_CreateAndCompileShader(char *shader_source, int shader_type)
{
    unsigned int shader = 0;
    int status = 0;
    static char shader_info_log[2048];

    if(shader_type == GL_VERTEX_SHADER || shader_type == GL_FRAGMENT_SHADER)
    {
        shader = glCreateShader(shader_type);
        glShaderSource(shader, 1, (GLchar **)&shader_source, NULL);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if(!status)
        {
            glGetShaderInfoLog(shader, 2048, NULL, shader_info_log);
            printf("r_CreateAndCompileShader: shader compilation failed! Reason:\n%s\n", shader_info_log);
            glDeleteShader(shader);
            shader = 0;
        }
    }
    else
    {
        printf("r_CreateAndCompileShader: unknown shader type.\n");
    }

    return shader;
}

unsigned int r_CreateAndLinkProgram(unsigned int vertex_shader, unsigned int fragment_shader)
{
    unsigned int program = 0;
    int status = 0;
    static char program_info_log[2048];

    if(vertex_shader && fragment_shader)
    {
        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);

        glGetProgramiv(program, GL_LINK_STATUS, &status);

        if(!status)
        {
            glGetProgramInfoLog(program, 2048, NULL, program_info_log);
            printf("r_CreateAndLinkProgram: program linking failed! Reason:\n%s\n", program_info_log);
            glDeleteProgram(program);
            program = 0;
        }
    }
    else
    {
        if(!vertex_shader)
        {
            printf("r_CreateAndLinkProgram: invalid vertex shader suplied!\n");
        }

        if(!fragment_shader)
        {
            printf("r_CreateAndLinkProgram: invalid fragment shader suplied!\n");
        }
    }

    return program;
}

#ifdef __cplusplus
}
#endif



