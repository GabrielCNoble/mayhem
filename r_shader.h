#ifndef R_SHADER_H
#define R_SHADER_H



enum R_SHADER_DEFAULT_UNIFORMS
{
    r_ProjectionMatrix = 0,
    r_ModelViewMatrix,
    r_ModelViewProjectionMatrix,
    r_BaseColor,
    r_LAST_UNIFORM,
};

struct uniform_t
{
    int location;
};

struct named_uniform_t
{
    struct uniform_t uniform;
    char *name;
};

struct shader_t
{
    unsigned int program;
    unsigned int vertex_shader;
    unsigned int fragment_shader;

    int vertex_position;
    int vertex_normal;

    struct uniform_t *default_uniforms;

    char *name;
};



int r_CreateShader(char *name);

void r_DestroyShader(int shader_handle);

char *r_LoadShaderSource(char *file_name);

int r_CompileShaderSource(int shader_handle, char *source);

int r_LoadShader(char *file_name);

int r_GetShader(char *name);

struct shader_t *r_GetShaderPointer(int shader_handle);

void r_SetShader(int shader_handle);



void r_DefaultUniformMatrix4fv(int uniform, float *value);

void r_UniformMatrix4fv(struct uniform_t *uniform, float *value);

void r_DefaultUniform4fv(int uniform, float *value);

void r_Uniform4fv(struct uniform_t *uniform, float *value);


#endif // R_SHADER_H









