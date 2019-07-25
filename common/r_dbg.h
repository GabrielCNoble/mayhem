#ifndef R_DBG_H
#define R_DBG_H

#include "../gmath/vector.h"


/* those functions serves only as an interface. All they do is
take the parameters and queue a backend command...  */

void r_InitDebug();

void r_ShutdownDebug();

void r_DrawLine(vec3_t a, vec3_t b, vec3_t color);

void r_DrawTriangle(vec3_t a, vec3_t b, vec3_t c, vec3_t color, int wireframe, int backface_culling, int depth_test);

void r_DrawCapsule(vec3_t position, float height, float radius);

void r_DrawPoint(vec3_t position, vec3_t color, float size, int smooth);


#endif // R_DBG_H
