#ifndef GUI_H
#define GUI_H

#include "libs/ImGui/include/imgui.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

int32_t gui_Init();

void gui_Shutdown();

void gui_BeginFrame();

void gui_EndFrame();

#ifdef __cplusplus
}
#endif

#endif // R_GUI_H
