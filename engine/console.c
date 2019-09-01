#include "console.h"
#include "r_common.h"
#include "input.h"
#include "gui.h"
#include "../common/containers/list.h"
#include "SDL2/SDL_atomic.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CON_MAX_LOGIC_LINES 64
#define CON_BUFFER_SIZE 0xffff
#define CON_MAX_VERTICAL_TRANSLATION 250.0

float con_vertical_translation = 0.0;
int con_console_state = 0;

char *con_buffer = NULL;
char *con_aux_buffer = NULL;
int con_next_logic_line = 0;
int con_logic_line_buffer_full = 0;
struct console_logic_line_t *con_logic_lines_buffer = NULL;
SDL_SpinLock con_spinlock;

struct list_t con_cvars;

extern struct renderer_t r_renderer;

char *test = "TEST OF THOSE BIG BALLS!!!!!";

void con_Test(char *arg)
{
    con_Printf("FUCK %s", arg);
}

void con_Exit()
{

}

void con_TestArgs(char *args)
{
    struct carg_t arg_array[16];
    int32_t arg_count;
    int32_t i;

    int32_t int_arg;
    float float_arg;

    con_ParseArgs(args, arg_array, &arg_count, 16);

    for(i = 0; i < arg_count; i++)
    {
        switch(arg_array[i].type)
        {
            case CARG_TYPE_STRING:
                con_Printf("string: %s", arg_array[i].arg.string_arg);
            break;

            case CARG_TYPE_INT:
//                memcpy(&int_arg, arg_array[i].arg_buffer, sizeof(int32_t));
                con_Printf("int: %d", arg_array[i].arg.int_arg);
            break;

            case CARG_TYPE_FLOAT:
//                memcpy(&float_arg, arg_array[i].arg_buffer, sizeof(float));
                con_Printf("float: %f", arg_array[i].arg.float_arg);
            break;
        }
    }
}

void con_IpPort(char *args)
{
    struct carg_t arg_array[2];

    con_ParseArgs(args, arg_array, NULL, 2);

    if(arg_array[0].type == CARG_TYPE_STRING && arg_array[1].type == CARG_TYPE_INT)
    {
        con_Printf("%s:%d", arg_array[0].arg.string_arg, arg_array[1].arg.int_arg);
    }
}

void con_SendMessage(char *message)
{
    con_Printf("> %s", message);
}

#ifdef __cplusplus
extern "C"
{
#endif

int32_t con_Init()
{
    con_buffer = (char *)calloc(CON_BUFFER_SIZE, 1);
    con_aux_buffer = (char *)calloc(CON_BUFFER_SIZE, 1);
    con_logic_lines_buffer = (struct console_logic_line_t *)calloc(sizeof(struct console_logic_line_t), CON_MAX_LOGIC_LINES);
    con_cvars.init(sizeof(struct cvar_t), 64);

    SDL_AtomicUnlock(&con_spinlock);

    con_CreateCVar("Test", CVAR_TYPE_STRING, test);
    con_CreateCVar("Print", CVAR_TYPE_FUNCTION, (void *)con_Test);
    con_CreateCVar("args", CVAR_TYPE_FUNCTION, (void *)con_TestArgs);
    con_CreateCVar("IpPort", CVAR_TYPE_FUNCTION, (void *)con_IpPort);
    con_CreateCVar("msg", CVAR_TYPE_FUNCTION, (void *)con_SendMessage);

    return 0;
}

void con_Shutdown()
{
    free(con_buffer);
    free(con_logic_lines_buffer);
}

static char con_input_buffer[1024];

void con_UpdateConsole()
{
    float window_height;
    float cursor_y;
    float content_width;
    int first_logic_line;
    int logic_line_index;

    int end_char_count;
    char *aux_buffer;
//    buff[0] = '\0';

    struct console_logic_line_t *logic_line;

    ImVec2 text_size;



    if(in_GetConKeyJustPressed())
    {
        con_ToggleConsole();
    }

    if(con_console_state)
    {
        if(con_vertical_translation < CON_MAX_VERTICAL_TRANSLATION)
        {
            con_vertical_translation += 7.5;
        }
    }
    else
    {
        if(con_vertical_translation > 0.0)
        {
            con_vertical_translation -= 7.5;
        }
    }



    if(con_vertical_translation > 0.0)
    {
        ImGui::SetNextWindowPos(ImVec2(0.0, con_vertical_translation), 0, ImVec2(0.0, 1.0));
        ImGui::SetNextWindowSize(ImVec2(r_renderer.renderer_width, r_renderer.renderer_height));
        ImGui::SetNextWindowFocus();
        if(ImGui::Begin("Console", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar))
        {
            SDL_AtomicLock(&con_spinlock);

            content_width = ImGui::GetWindowContentRegionWidth() - 2.0;
            window_height = ImGui::GetWindowHeight();

            ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(100, 0, 0, 100));
            ImGui::BeginChild("Interactions", ImVec2(content_width, window_height - 65.0));

            first_logic_line = con_next_logic_line - 1;

            if(first_logic_line < 0)
            {
                if(con_logic_line_buffer_full)
                {
                    first_logic_line = CON_MAX_LOGIC_LINES - 1;
                }
                else
                {
                    first_logic_line = 0;
                }
            }

            logic_line_index = first_logic_line;

            cursor_y = ImGui::GetWindowHeight() - 10.0;

            do
            {
                logic_line = con_logic_lines_buffer + logic_line_index;

                if(logic_line->start + logic_line->length >= CON_BUFFER_SIZE)
                {
                    end_char_count = CON_BUFFER_SIZE - logic_line->start;
                    memcpy(con_aux_buffer, con_buffer + logic_line->start, end_char_count);
                    memcpy(con_aux_buffer + end_char_count, con_buffer, logic_line->length - end_char_count);
                    aux_buffer = con_aux_buffer;
                }
                else
                {
                    aux_buffer = con_buffer + logic_line->start;
                }

                text_size = ImGui::CalcTextSize(aux_buffer, NULL, false, 0.0);
                text_size.y = text_size.y * (1 + (int)(text_size.x / content_width));
                cursor_y -= text_size.y + 8.0;
                ImGui::SetCursorPosY(cursor_y);
                ImGui::TextWrapped("%s", aux_buffer);
                logic_line_index = logic_line->prev;
            }
            while(logic_line_index != first_logic_line && logic_line_index != -1);

            SDL_AtomicUnlock(&con_spinlock);

            ImGui::EndChild();
            ImGui::PopStyleColor();

            ImGui::SetNextItemWidth(content_width);
            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30.0);
            ImGui::SetKeyboardFocusHere();
            if(ImGui::InputText("", con_input_buffer, 1024, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                con_ProcessInput(con_input_buffer);
                con_input_buffer[0] = '\0';
            }
        }

        ImGui::End();
    }
    else
    {

    }
}

void con_ToggleConsole()
{
    con_console_state = !con_console_state;
}

void con_OpenConsole()
{
    con_console_state = 1;
}

void con_CloseConsole()
{
    con_console_state = 0;
}


void con_ProcessInput(char *input)
{
    static char command[512];
    int command_index = 0;
    int i;

    struct cvar_t *cvar;

    i = 0;

    while(input[i] != '\0' && input[i] != ' ' && input[i] != '\n')
    {
        command[i] = input[i];
        i++;
    }

    command[i] = '\0';

    cvar = con_GetCVar(command);

    if(cvar)
    {
        while(input[i] == ' ')i++;

        if(cvar->type == CVAR_TYPE_FUNCTION)
        {
            ((void (*)(char *))cvar->value)(input + i);
        }
        else
        {
            con_PrintCVar(cvar);
        }

        return;
    }
    else
    {
//        con_Printf(input);
        con_SendMessage(input);
    }
}

__declspec(dllexport) __stdcall void con_Printf(const char *fmt, ...)
{
    struct console_logic_line_t *logic_line;
    struct console_logic_line_t *prev_logic_line;
    int next_logic_line_index;
    int prev_logic_line_index;
    int i;
    int output_offset;

//    printf("enter con_Printf\n");

    SDL_AtomicLock(&con_spinlock);

    va_list args;
    va_start(args, fmt);

    logic_line = con_logic_lines_buffer + con_next_logic_line;
    logic_line->start = 0;
    logic_line->length = 0;
    logic_line->time = 0;

    prev_logic_line_index = con_next_logic_line - 1;

    if(prev_logic_line_index < 0)
    {
        /* went past index 0... */
        if(con_logic_line_buffer_full)
        {
            /* the buffer is full, so wrap the index to
            its end.*/
            prev_logic_line_index = CON_MAX_LOGIC_LINES - 1;
        }
        else
        {
            /* buffer not full yet and prev index < 0. Means
            this is the first interaction. */
            prev_logic_line_index = -1;
        }
    }

    if(prev_logic_line_index >= 0)
    {
        /* if there's a valid previous interaction, use it to find
        where starts the char buffer space for this interaction. */
        prev_logic_line = con_logic_lines_buffer + prev_logic_line_index;
        logic_line->start = (prev_logic_line->start + prev_logic_line->length) % CON_BUFFER_SIZE;
    }

    vsnprintf(con_aux_buffer, CON_BUFFER_SIZE, fmt, args);
    output_offset = logic_line->start;

    for(i = 0; con_aux_buffer[i]; i++)
    {
        /* make sure the copy wraps around... */
        con_buffer[output_offset] = con_aux_buffer[i];
        output_offset = (output_offset + 1) % CON_BUFFER_SIZE;
    }

    con_buffer[output_offset] = '\0';

    logic_line->length = strlen(con_aux_buffer) + 1;

    logic_line->prev = prev_logic_line_index;

    next_logic_line_index = (con_next_logic_line+ 1) % CON_MAX_LOGIC_LINES;

    if(next_logic_line_index < con_next_logic_line)
    {
        /* the next index is smaller than the current one,
        which means it wrapped around the buffer. The buffer is
        now full. */
        con_logic_line_buffer_full = 1;
    }

    con_next_logic_line = next_logic_line_index;

    SDL_AtomicUnlock(&con_spinlock);

//    printf("exit con_Printf\n");
}


__declspec(dllexport) __stdcall void con_CreateCVar(char *name, int type, void *value)
{
    int index;
    struct cvar_t *cvar;

    index = con_cvars.add(NULL);
    cvar = (struct cvar_t *)con_cvars.get(index);

    strcpy(cvar->name, name);
    cvar->type = type;
    cvar->value = value;
}

__declspec(dllexport) __stdcall struct cvar_t *con_GetCVar(char *name)
{
    unsigned int i;
    struct cvar_t *cvar;

    for(i = 0; i < con_cvars.cursor; i++)
    {
        cvar = (struct cvar_t *)con_cvars.get(i);

        if(!strcmp(cvar->name, name))
        {
            return cvar;
        }
    }

    return NULL;
}

void con_PrintCVar(struct cvar_t *cvar)
{
    if(cvar)
    {
        switch(cvar->type)
        {
            case CVAR_TYPE_STRING:
                con_Printf("%s", cvar->value);
            break;

            case CVAR_TYPE_INT:
                con_Printf("%d", *(int *)cvar->value);
            break;
        }
    }
}

void con_SetCVarValue(struct cvar_t *cvar, ...)
{
    va_list args;

    int *int_cvar;
    char *str_cvar;

    if(cvar)
    {
        va_start(args, cvar);

        switch(cvar->type)
        {
            case CVAR_TYPE_CHAR:

            break;

            case CVAR_TYPE_INT:
                int_cvar = (int *)cvar->value;
                *int_cvar = va_arg(args, int);
            break;

            case CVAR_TYPE_STRING:
                str_cvar = (char *)cvar->value;
                strcpy(str_cvar, va_arg(args, char *));
            break;
        }
    }
}

int con_IsConsoleOpen()
{
    return con_vertical_translation > 0.0;
}

uint32_t con_ParseArg(char *str, struct carg_t *arg)
{
    uint32_t index = 0;
    int32_t arg_cursor = 0;
    uint32_t int_arg;
    float float_arg;
    char arg_str[128];
    char stop;

    arg->type = CARG_TYPE_NONE;

    while(str[index] == ' ') index++;

    if(str[index] == '\0')
    {
        return 0;
    }

    if(str[index] == '"')
    {
        index++;
        arg->type = CARG_TYPE_STRING;
        stop = '"';
    }
    else
    {
        stop = ' ';
    }

    while(str[index] != stop && str[index] != '\0')
    {
        arg_str[arg_cursor] = str[index];
        arg_cursor++;
        index++;
    }

    arg_str[arg_cursor] = '\0';

    if(arg->type == CARG_TYPE_STRING)
    {
        strcpy(arg->arg.string_arg, arg_str);

        if(str[index] != '"')
        {
            arg->type = CARG_TYPE_NONE;
        }

        index++;
    }
    else
    {
        arg->type = CARG_TYPE_INT;

        for(; arg_cursor >= 0; arg_cursor--)
        {
            if(arg_str[arg_cursor] == '.')
            {
                arg->type = CARG_TYPE_FLOAT;
                break;
            }
        }

        switch(arg->type)
        {
            case CARG_TYPE_INT:
                arg->arg.int_arg = atoi(arg_str);
            break;

            case CARG_TYPE_FLOAT:
                arg->arg.float_arg = atof(arg_str);
            break;
        }
    }
    return index;
}

void con_ParseArgs(char *str, struct carg_t *args, int32_t *arg_count, int32_t max_args)
{
    int32_t offset = 0;
    int32_t count = 0;
    struct carg_t arg;

    for(; offset < max_args; offset++)
    {
        args[offset].type = CARG_TYPE_NONE;
    }

    offset = 0;

    while(str[offset] && count < max_args)
    {
        offset += con_ParseArg(str + offset, &arg);

        if(arg.type == CARG_TYPE_NONE)
        {
            break;
        }

        args[count] = arg;
        count++;
    }

    if(arg_count)
    {
        *arg_count = count;
    }
}


#ifdef __cplusplus
}
#endif








