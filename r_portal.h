#ifndef R_PORTAL_H
#define R_PORTAL_H

#include "r_view.h"
#include "vector.h"
#include "matrix.h"

struct portal_t
{
    struct view_t *view;

    vec2_t size;
};


#endif // R_PORTAL_H
