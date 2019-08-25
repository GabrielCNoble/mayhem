#include "console.h"
#include "r_common.h"
#include "input.h"
#include "gui.h"
#include "../common/containers/list.h"

#include <stdlib.h>
#include <stdio.h>

#define CON_MAX_LOGIC_LINES 64
#define CON_BUFFER_SIZE 0xffff

float con_vertical_translation = 0.0;
int con_console_state = 0;

char *con_buffer = NULL;
char *con_aux_buffer = NULL;
int con_next_logic_line = 0;
int con_logic_line_buffer_full = 0;
struct console_logic_line_t *con_logic_lines_buffer = NULL;

struct list_t con_cvars;

extern struct renderer_t r_renderer;

char *test = "TEST OF THOSE BIG BALLS!!!!!";

void con_Test()
{
    con_Printf("FUCK");
}

void con_Init()
{
    con_buffer = (char *)calloc(CON_BUFFER_SIZE, 1);
    con_aux_buffer = (char *)calloc(CON_BUFFER_SIZE, 1);
    con_logic_lines_buffer = (struct console_logic_line_t *)calloc(sizeof(struct console_logic_line_t), CON_MAX_LOGIC_LINES);
    con_cvars.init(sizeof(struct cvar_t), 64);

    con_CreateCVar("Test", CVAR_TYPE_STRING, test);
    con_CreateCVar("Print", CVAR_TYPE_FUNCTION, (void *)con_Test);
}

void con_Shutdown()
{
    free(con_buffer);
    free(con_logic_lines_buffer);
}

void con_UpdateConsole()
{
    if(in_GetConKeyJustPressed())
    {
        con_ToggleConsole();
    }

    if(con_console_state)
    {
        if(con_vertical_translation < 250.0)
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
}

void con_DrawConsole()
{
    float window_height;
    float cursor_y;
    float content_width;
    int first_logic_line;
    int logic_line_index;
    char buff[1024];
    int end_char_count;
    char *aux_buffer;
    buff[0] = '\0';

    struct console_logic_line_t *logic_line;

    ImVec2 text_size;

    if(con_vertical_translation > 0.0)
    {
        ImGui::SetNextWindowPos(ImVec2(0.0, con_vertical_translation), 0, ImVec2(0.0, 1.0));
        ImGui::SetNextWindowSize(ImVec2(r_renderer.renderer_width, r_renderer.renderer_height));
        if(ImGui::Begin("Console", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar))
        {
            content_width = ImGui::GetWindowContentRegionWidth();
            window_height = ImGui::GetWindowHeight();

            ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 70, 140, 100));
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

            ImGui::EndChild();
            ImGui::PopStyleColor();

            ImGui::SetNextItemWidth(content_width);
            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30.0);
            ImGui::SetKeyboardFocusHere();
            if(ImGui::InputText("", buff, 1024, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                con_ProcessInput(buff);
            }
        }

        ImGui::End();
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

        if(input[i] == '\0')
        {
            if(cvar->type == CVAR_TYPE_FUNCTION)
            {
                ((void (*)())cvar->value)();
            }
            else
            {
                con_PrintCVar(cvar);
            }
        }
        else
        {

        }

        return;
    }
    else
    {
        con_Printf(input);
    }
}

void con_Printf(const char *fmt, ...)
{
    struct console_logic_line_t *logic_line;
    struct console_logic_line_t *prev_logic_line;
    int next_logic_line_index;
    int prev_logic_line_index;
    int i;
    int output_offset;

    va_list args;
    va_start(args, fmt);

    logic_line = con_logic_lines_buffer + con_next_logic_line;
    logic_line->start = 0;
    logic_line->length = 0;

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
        logic_line->start = prev_logic_line->start + prev_logic_line->length;
    }

    vsnprintf(con_aux_buffer, CON_BUFFER_SIZE, fmt, args);
    output_offset = logic_line->start;

    for(i = 0; con_aux_buffer[i]; i++)
    {
        /* make sure the copy wraps around... */
        con_buffer[output_offset % CON_BUFFER_SIZE] = con_aux_buffer[i];
        output_offset++;
    }

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
}


void con_CreateCVar(char *name, int type, void *value)
{
    int index;
    struct cvar_t *cvar;

    index = con_cvars.add(NULL);
    cvar = (struct cvar_t *)con_cvars.get(index);

    strcpy(cvar->name, name);
    cvar->type = type;
    cvar->value = value;
}

struct cvar_t *con_GetCVar(char *name)
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














