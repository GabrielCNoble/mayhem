#ifndef R_SHADER_H
#define R_SHADER_H



enum R_SHADER_DEFAULT_UNIFORMS
{
    r_ModelViewProjectionMatrix,
    r_ViewProjectionMatrix,
    r_ViewMatrix,
    r_ViewModelMatrix,
    r_BaseColor,
    r_DebugColor,
    r_MaterialFlags,
    r_NearPlane,
    //r_LightCount,

    r_TextureSampler0,
    r_TextureSampler1,

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
    int vertex_tangent;
    int vertex_tex_coords;
    int vertex_color;

    struct uniform_t *default_uniforms;

    char *name;
};

void r_InitShader();

void r_ShutdownShader();

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

void r_DefaultUniform1i(int uniform, int value);

void r_Uniform4fv(struct uniform_t *uniform, float *value);

void r_Uniform1i(struct uniform_t *uniform, int value);



unsigned int r_CreateAndCompileShader(char *shader_source, int shader_type);

unsigned int r_CreateAndLinkProgram(unsigned int vertex_shader, unsigned int fragment_shader);

#endif // R_SHADER_H








