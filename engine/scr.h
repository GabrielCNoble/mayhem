#ifndef SCR_H
#define SCR_H

#include <stdint.h>

struct script_t
{
    struct script_t *next;
    void (*compile_callback)();
    void (*setup_callback)();
};

void scr_Init();

void scr_Shutdown();




#endif // SCR_H
