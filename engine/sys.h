#ifndef SYS_H
#define SYS_H

#include <stdint.h>

#include "r_main.h"
#include "net.h"
#include "ent.h"
#include "gui.h"
#include "console.h"
#include "res.h"
#include "player.h"
#include "physics.h"
#include "input.h"
#include "w_main.h"

enum SYS_SUBSYSTEMS
{
    SYS_SUBSYSTEM_RENDERER = 1,
    SYS_SUBSYSTEM_AUDIO = 1 << 1,
    SYS_SUBSYSTEM_ENTITY = 1 << 2,
    SYS_SUBSYSTEM_PHYSICS = 1 << 3,
    SYS_SUBSYSTEM_RESOURCE = 1 << 4,
    SYS_SUBSYSTEM_GUI = 1 << 5,
    SYS_SUBSYSTEM_INPUT = 1 << 6,
    SYS_SUBSYSTEM_NET = 1 << 7,
    SYS_SUBSYSTEM_CONSOLE = 1 << 8,
    SYS_SUBSYSTEM_PLAYER = 1 << 9,
    SYS_SUBSYSTEM_WORLD = 1 << 10,
};

#ifdef __cplusplus
extern "C"
{
#endif

int32_t sys_Init();

int32_t sys_InitSubsystems(uint32_t subsystems);

void sys_Shutdown();

void sys_SetInitCallback(void (*init_callback)());

void sys_SetShutdownCallback(void (*shutdown_callback)());

void sys_SetGameMainCallback(void (*game_main_callback)());

int sys_Main(void *data);

void sys_MainLoop();

void sys_SetLastError(char *msg);

char *sys_GetLastError();

void sys_Testvec3_t(vec3_t vec);


//int fe_Frontend(void *data);
//
//void fe_RunFrontend();

#ifdef __cplusplus
}
#endif

#endif // SYS_H
