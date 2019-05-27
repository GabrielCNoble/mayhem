#include "input.h"

#include "SDL2\SDL.h"
#include "SDL2\SDL_mouse.h"
#include "r_common.h"

#include "list.h"

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


void in_UpdateInput()
{
    int mouse_x;
    int mouse_y;

    unsigned char *keys;

    int key;

    int i;

    while(SDL_PollEvent(&event))
    {

    }

    keys = (unsigned char *)SDL_GetKeyboardState(NULL);

    for(i = 0; i < registered_cursor; i++)
    {
        key = registered_keys[i];

        key_status[key] &= ~(KEY_STATUS_JUST_PRESSED | KEY_STATUS_JUST_RELEASED);

        if(keys[key])
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


    SDL_GetMouseState(&mouse_x, &mouse_y);
    mouse_y = r_renderer.window_height - mouse_y;

    prev_normalized_mouse_x = normalized_mouse_x;
    prev_normalized_mouse_y = normalized_mouse_y;

    normalized_mouse_x = ((float)mouse_x / (float)r_renderer.window_width) * 2.0 - 1.0;
    normalized_mouse_y = ((float)mouse_y / (float)r_renderer.window_height) * 2.0 - 1.0;

    normalized_mouse_dx = normalized_mouse_x;
    normalized_mouse_dy = normalized_mouse_y;

    SDL_WarpMouseInWindow(r_renderer.window, r_renderer.window_width / 2, r_renderer.window_height / 2);
    SDL_ShowCursor(0);
}

vec2_t in_GetMouseDelta()
{
    return vec2_t(normalized_mouse_dx, normalized_mouse_dy);
}

void in_RegisterKey(int key)
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

void in_UnregisterKey(int key)
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

int in_GetKeyStatus(int key)
{
    return key_status[key];
}








