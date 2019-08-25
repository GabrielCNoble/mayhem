#ifndef CONSOLE_H
#define CONSOLE_H

struct console_logic_line_t
{
    unsigned short start;
    unsigned short length;
    unsigned int prev;
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

void con_Init();

void con_Shutdown();

void con_UpdateConsole();

void con_DrawConsole();

void con_ToggleConsole();

void con_OpenConsole();

void con_CloseConsole();

void con_ProcessInput(char *input);

void con_Printf(const char *fmt, ...);

void con_CreateCVar(char *name, int type, void *value);

struct cvar_t *con_GetCVar(char *name);

void con_PrintCVar(struct cvar_t *cvar);

void con_SetCVarValue(struct cvar_t *cvar, ...);



#endif // CONSOLE_H
