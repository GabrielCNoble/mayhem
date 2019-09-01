#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

struct console_logic_line_t
{
    uint16_t start;
    uint16_t length;
    uint32_t prev;
    uint32_t time;
};

enum CVAR_TYPE
{
    CVAR_TYPE_CHAR = 0,
    CVAR_TYPE_SHORT,
    CVAR_TYPE_INT,
    CVAR_TYPE_LONG,
    CVAR_TYPE_FLOAT,
    CVAR_TYPE_DOUBLE,
    CVAR_TYPE_VEC2,
    CVAR_TYPE_VEC3,
    CVAR_TYPE_VEC4,
    CVAR_TYPE_STRING,
    CVAR_TYPE_FUNCTION,
};

struct cvar_t
{
    char name[32];
    int type;
    void *value;
};

enum CARG_TYPE
{
    CARG_TYPE_NONE = 0,
    CARG_TYPE_INT,
    CARG_TYPE_FLOAT,
    CARG_TYPE_STRING,
};

union carg_data_t
{
    char string_arg[127];
    int32_t int_arg;
    float float_arg;
};

struct carg_t
{
    union carg_data_t arg;
    uint8_t type;
};

#ifdef __cplusplus
extern "C"
{
#endif

int32_t con_Init();

void con_Shutdown();

void con_UpdateConsole();

void con_ToggleConsole();

void con_OpenConsole();

void con_CloseConsole();

void con_ProcessInput(char *input);

void con_Printf(const char *fmt, ...);

void con_CreateCVar(char *name, int type, void *value);

struct cvar_t *con_GetCVar(char *name);

void con_PrintCVar(struct cvar_t *cvar);

void con_SetCVarValue(struct cvar_t *cvar, ...);

int con_IsConsoleOpen();


uint32_t con_ParseArg(char *str, struct carg_t *arg);

void con_ParseArgs(char *str, struct carg_t *args, int32_t *arg_count, int32_t max_args);

#ifdef __cplusplus
}
#endif




#endif // CONSOLE_H
