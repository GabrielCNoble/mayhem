#ifndef R_SHADER_H
#define R_SHADER_H




struct shader_t
{
    unsigned int program;
    unsigned int vertex_shader;
    unsigned int fragment_shader;

    char *name;
};



int r_CreateShader(char *name);

void r_DestroyShader(int shader_handle);

char *r_LoadShaderSource(char *file_name);

int r_CompileShaderSource(int shader_handle, char *source);

int r_LoadShader(char *file_name);

struct shader_t *r_GetShaderPointer(int shader_handle);


#endif // R_SHADER_H
