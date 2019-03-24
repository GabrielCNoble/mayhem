#ifndef INPUT_H
#define INPUT_H


#include "vector.h"


enum KEY_STATUS
{
    KEY_STATUS_NONE = 0,
    KEY_STATUS_PRESSED = 1,
    KEY_STATUS_JUST_PRESSED = 1 << 1,
    KEY_STATUS_JUST_RELEASED = 1 << 2,
};

void in_UpdateInput();

vec2_t in_GetMouseDelta();

void in_RegisterKey(int key);

void in_UnregisterKey(int key);

int in_GetKeyStatus(int key);


#endif // INPUT_H
