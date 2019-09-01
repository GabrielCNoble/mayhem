#ifndef INPUT_H
#define INPUT_H


#include "../common/gmath/vector.h"

enum KEY_STATUS
{
    KEY_STATUS_NONE = 0,
    KEY_STATUS_PRESSED = 1,
    KEY_STATUS_JUST_PRESSED = 1 << 1,
    KEY_STATUS_JUST_RELEASED = 1 << 2,
};

enum MOUSE_STATUS
{
    MOUSE_STATUS_LEFT_BUTTON_PRESSED = 1,
    MOUSE_STATUS_LEFT_BUTTON_JUST_PRESSED = 1 << 1,
    MOUSE_STATUS_LEFT_BUTTON_JUST_RELEASED = 1 << 2,

    MOUSE_STATUS_MIDDLE_BUTTON_PRESSED = 1 << 3,
    MOUSE_STATUS_MIDDLE_BUTTON_JUST_PRESSED = 1 << 4,
    MOUSE_STATUS_MIDDLE_BUTTON_JUST_RELEASED = 1 << 5,

    MOUSE_STATUS_RIGHT_BUTTON_PRESSED = 1 << 6,
    MOUSE_STATUS_RIGHT_BUTTON_JUST_PRESSED = 1 << 7,
    MOUSE_STATUS_RIGHT_BUTTON_JUST_RELEASED = 1 << 8,
};

#ifdef __cplusplus
extern "C"
{
#endif

void in_UpdateInput();

void in_be_UpdateInput();

vec2_t in_GetMousePos();

vec2_t in_GetMouseDelta();

void in_RegisterKey(int key);

void in_UnregisterKey(int key);

int in_GetKeyStatus(int key);

int in_GetMouseStatus();

int in_GetConKeyJustPressed();

unsigned char *in_GetKeys();

void in_TextInput(int enable);

unsigned char *in_GetTextInputBuffer();

void in_ClearTextInputBuffer();

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
