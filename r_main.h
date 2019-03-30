#ifndef R_MAIN_H
#define R_MAIN_H

#include "r_common.h"
#include "r_view.h"
#include "r_frame.h"
#include "r_verts.h"
#include "r_shader.h"

#ifdef __cplusplus
extern "C"
{
#endif

void r_Init();

void r_Shutdown();

void r_SetWindowSize(int width, int height);

void r_SetRendererResolution(int width, int height);




#ifdef __cplusplus
}
#endif



#endif // R_MAIN_H
