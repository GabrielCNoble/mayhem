#include "input.h"

#include "SDL2\SDL.h"
#include "SDL2\SDL_mouse.h"
//#include "be_backend.h"
#include "r_common.h"
#include "r_main.h"

#include <stdio.h>

#include "../common/containers/list.h"

#define IN_TEXT_INPUT_BUFFER_SIZE 1024

extern struct renderer_t r_renderer;

static SDL_Event event;
static float normalized_mouse_x;
static float normalized_mouse_y;

static float prev_normalized_mouse_x;
static float prev_normalized_mouse_y;

float static normalized_mouse_dx;
float static normalized_mouse_dy;

int registered_cursor = 0;
unsigned short key_status[SDL_NUM_SCANCODES] = {KEY_STATUS_NONE};
int registered_keys[SDL_NUM_SCANCODES];

unsigned int in_mouse_status = 0;

int in_con_key_just_pressed = 0;
int in_con_key_pressed = 0;

int in_text_input_enabled = 0;
unsigned char in_text_input_buffer[IN_TEXT_INPUT_BUFFER_SIZE];
int in_text_input_buffer_cursor = 0;

unsigned char *in_keys = NULL;

#ifdef __cplusplus
extern "C"
{
#endif

void in_UpdateInput()
{
    r_QueueCmd(R_CMD_UPDATE_INPUT, NULL, 0);
    r_WaitEmptyQueue();
}

void in_be_UpdateInput()
{
    int mouse_x;
    int mouse_y;
    int mouse_status;

//    unsigned char *keys;

    int key;
    int mod;

    int i;

    in_con_key_just_pressed = 0;

    while(SDL_PollEvent(&event))
    {
        mod = 0;

        if(event.key.keysym.mod & KMOD_LSHIFT)
        {
            mod |= 32;
        }
        else
        {
            mod &= ~32;
        }

        if(event.key.keysym.mod & KMOD_ALT)
        {
            mod |= 16;
        }
        else
        {
            mod &= ~16;
        }

        switch(event.type)
        {
            case SDL_KEYDOWN:

                if(in_text_input_buffer_cursor >= IN_TEXT_INPUT_BUFFER_SIZE - 1)
                {
                    break;
                }

                switch(event.key.keysym.sym)
                {
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        continue;
                    break;

                    case SDLK_KP_1:
                        mod = 0;
                    case SDLK_1:
                        key = (mod & 32) ? '!' : '1';
                    break;

                    case SDLK_KP_2:
                        mod = 0;
                    case SDLK_2:
                        key = (mod & 32) ? '@' : '2';
                    break;

                    case SDLK_KP_3:
                        mod = 0;
                    case SDLK_3:
                        key = (mod & 32) ? '#' : '3';
                    break;

                    case SDLK_KP_4:
                        mod = 0;
                    case SDLK_4:
                        key = (mod & 32) ? '$' : '4';
                    break;

                    case SDLK_KP_5:
                        mod = 0;
                    case SDLK_5:
                        key = (mod & 32) ? '%' : '5';
                    break;

                    case SDLK_KP_6:
                        mod = 0;
                    case SDLK_6:
                        key = (mod & 32) ? '¨' : '6';
                    break;

                    case SDLK_KP_7:
                        mod = 0;
                    case SDLK_7:
                        key = (mod & 32) ? '&' : '7';
                    break;

                    case SDLK_KP_8:
                        mod = 0;
                    case SDLK_8:
                        key = (mod & 32) ? '*' : '8';
                    break;

                    case SDLK_KP_9:
                        mod = 0;
                    case SDLK_9:
                        key = (mod & 32) ? '(' : '9';
                    break;

                    case SDLK_KP_0:
                        mod = 0;
                    case SDLK_0:
                        key = (mod & 32) ? ')' : '0';
                    break;


                    case SDLK_KP_MINUS:
                        mod = 0;
                    case SDLK_MINUS:
                        key = (mod & 32) ? '_' : '-';
                    break;

                    case SDLK_KP_PLUS:
                        mod = 0;
                    case SDLK_PLUS:
                        key = (mod & 32) ? '=' : '+';
                    break;

                    case SDLK_KP_MULTIPLY:
                        key = '*';
                    break;

                    case SDLK_KP_DIVIDE:
                        key = '/';
                    break;

                    case SDLK_KP_PERIOD:
                        key = '.';
                    break;

                    case SDLK_COMMA:
                        key = (mod & 32) ? '<' : ',';
                    break;

                    case SDLK_PERIOD:
                        key = (mod & 32) ? '>' : '.';
                    break;

                    case SDLK_SEMICOLON:
                        key = (mod & 32) ? ':' : ';';
                    break;

                    case SDLK_KP_ENTER:
                        key = SDLK_RETURN;
                    break;

                    case SDLK_q:
                        key = (mod & 16) ? '/' : 'q';
                    break;

                    case SDLK_QUOTE:
                        key = (mod & 32) ? '"' : '\'';
                    break;

                    case SDLK_LALT:
                    case SDLK_RALT:
                        continue;
                    break;

                    default:
                        key = event.key.keysym.sym & (~mod);
                    break;
                }

                if(in_text_input_enabled)
                {
                    in_text_input_buffer[in_text_input_buffer_cursor] = key;
                    in_text_input_buffer_cursor++;
                }

            break;
        }

        in_text_input_buffer[in_text_input_buffer_cursor] = '\0';
    }

    in_keys = (unsigned char *)SDL_GetKeyboardState(NULL);

    /*
        console key check. Necessary to do it here, since we need to
        check for continuous key press.
    */
    if(in_keys[SDL_SCANCODE_GRAVE] && (!(in_keys[SDL_SCANCODE_LSHIFT])))
    {
        if(!in_con_key_pressed)
        {
            in_con_key_just_pressed = 1;
        }

        in_con_key_pressed = 1;
    }
    else
    {
        in_con_key_pressed = 0;
    }


    if(in_text_input_enabled)
    {
        for(i = 0; i < registered_cursor; i++)
        {
            key = registered_keys[i];
            key_status[key] &= ~(KEY_STATUS_JUST_PRESSED | KEY_STATUS_JUST_RELEASED | KEY_STATUS_PRESSED);
        }
    }
    else
    {
        for(i = 0; i < registered_cursor; i++)
        {
            key = registered_keys[i];

            key_status[key] &= ~(KEY_STATUS_JUST_PRESSED | KEY_STATUS_JUST_RELEASED);

            if(in_keys[key])
            {
                if(!(key_status[key] & KEY_STATUS_PRESSED))
                {
                    key_status[key] |= KEY_STATUS_JUST_PRESSED;
                }

                key_status[key] |= KEY_STATUS_PRESSED;
            }
            else
            {
                if(key_status[key] & KEY_STATUS_PRESSED)
                {
                    key_status[key] |= KEY_STATUS_JUST_RELEASED;
                }

                key_status[key] &= ~KEY_STATUS_PRESSED;
            }
        }
    }

    mouse_status = SDL_GetMouseState(&mouse_x, &mouse_y);
    mouse_y = r_renderer.window_height - mouse_y;

    prev_normalized_mouse_x = normalized_mouse_x;
    prev_normalized_mouse_y = normalized_mouse_y;

    normalized_mouse_x = ((float)mouse_x / (float)r_renderer.window_width) * 2.0 - 1.0;
    normalized_mouse_y = ((float)mouse_y / (float)r_renderer.window_height) * 2.0 - 1.0;

    if(!in_text_input_enabled)
    {
        normalized_mouse_dx = normalized_mouse_x;
        normalized_mouse_dy = normalized_mouse_y;
    }
    else
    {
        normalized_mouse_dx = 0.0;
        normalized_mouse_dy = 0.0;
    }

    in_mouse_status &= ~(MOUSE_STATUS_LEFT_BUTTON_JUST_PRESSED | MOUSE_STATUS_LEFT_BUTTON_JUST_RELEASED |
                         MOUSE_STATUS_MIDDLE_BUTTON_JUST_PRESSED | MOUSE_STATUS_MIDDLE_BUTTON_JUST_RELEASED |
                         MOUSE_STATUS_RIGHT_BUTTON_JUST_PRESSED | MOUSE_STATUS_RIGHT_BUTTON_JUST_RELEASED);

    if(mouse_status & SDL_BUTTON_LMASK)
    {
        if(!(in_mouse_status & MOUSE_STATUS_LEFT_BUTTON_PRESSED))
        {
            in_mouse_status |= MOUSE_STATUS_LEFT_BUTTON_JUST_PRESSED;
        }

        in_mouse_status |= MOUSE_STATUS_LEFT_BUTTON_PRESSED;
    }
    else
    {
        if(in_mouse_status & MOUSE_STATUS_LEFT_BUTTON_PRESSED)
        {
            in_mouse_status |= MOUSE_STATUS_LEFT_BUTTON_JUST_RELEASED;
        }

        in_mouse_status &= ~MOUSE_STATUS_LEFT_BUTTON_PRESSED;
    }



    if(mouse_status & SDL_BUTTON_MMASK)
    {
        if(!(in_mouse_status & MOUSE_STATUS_MIDDLE_BUTTON_PRESSED))
        {
            in_mouse_status |= MOUSE_STATUS_MIDDLE_BUTTON_JUST_PRESSED;
        }

        in_mouse_status |= MOUSE_STATUS_MIDDLE_BUTTON_PRESSED;
    }
    else
    {
        if(in_mouse_status & MOUSE_STATUS_MIDDLE_BUTTON_PRESSED)
        {
            in_mouse_status |= MOUSE_STATUS_MIDDLE_BUTTON_JUST_RELEASED;
        }

        in_mouse_status &= ~MOUSE_STATUS_MIDDLE_BUTTON_PRESSED;
    }



    if(mouse_status & SDL_BUTTON_RMASK)
    {
        if(!(in_mouse_status & MOUSE_STATUS_RIGHT_BUTTON_PRESSED))
        {
            in_mouse_status |= MOUSE_STATUS_RIGHT_BUTTON_JUST_PRESSED;
        }

        in_mouse_status |= MOUSE_STATUS_RIGHT_BUTTON_PRESSED;
    }
    else
    {
        if(in_mouse_status & MOUSE_STATUS_RIGHT_BUTTON_PRESSED)
        {
            in_mouse_status |= MOUSE_STATUS_RIGHT_BUTTON_JUST_RELEASED;
        }

        in_mouse_status &= ~MOUSE_STATUS_RIGHT_BUTTON_PRESSED;
    }

    if(!in_text_input_enabled)
    {
        SDL_WarpMouseInWindow(r_renderer.window, r_renderer.window_width / 2, r_renderer.window_height / 2);
        SDL_ShowCursor(0);
    }
    else
    {
        SDL_ShowCursor(1);
    }
}

__declspec(dllexport) __stdcall vec2_t in_GetMousePos()
{
    return vec2_t(normalized_mouse_x, normalized_mouse_y);
}

__declspec(dllexport) __stdcall vec2_t in_GetMouseDelta()
{
    return vec2_t(normalized_mouse_dx, normalized_mouse_dy);
}

__declspec(dllexport) __stdcall void in_RegisterKey(int key)
{
    int i;

    for(i = 0; i < registered_cursor; i++)
    {
        if(registered_keys[i] == key)
        {
            return;
        }
    }

    registered_keys[registered_cursor] = key;
    registered_cursor++;
}

__declspec(dllexport) __stdcall void in_UnregisterKey(int key)
{
    int i;

    for(i = 0; i < registered_cursor; i++)
    {
        if(registered_keys[i] == key)
        {
            registered_cursor--;

            if(i < registered_cursor)
            {
                registered_keys[i] = registered_keys[registered_cursor];
            }

            return;
        }
    }

    key_status[key] = KEY_STATUS_NONE;
}

__declspec(dllexport) __stdcall int in_GetKeyStatus(int key)
{
    return key_status[key];
}

__declspec(dllexport) __stdcall int in_GetMouseStatus()
{
    return in_mouse_status;
}

__declspec(dllexport) __stdcall int in_GetConKeyJustPressed()
{
    return in_con_key_just_pressed;
}


__declspec(dllexport) __stdcall unsigned char *in_GetKeys()
{
    return in_keys;
}

__declspec(dllexport) __stdcall void in_TextInput(int enable)
{
    in_text_input_enabled = enable;
}

__declspec(dllexport) __stdcall unsigned char *in_GetTextInputBuffer()
{
    return in_text_input_buffer;
}


__declspec(dllexport) __stdcall void in_ClearTextInputBuffer()
{
    in_text_input_buffer_cursor = 0;
    in_text_input_buffer[0] = '\0';
}

#ifdef __cplusplus
}
#endif





