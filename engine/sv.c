#include "sv.h"
#include "sys.h"

#include "../common/utils/file.h"

#ifdef __cplusplus
extern "C"
{
#endif

__declspec(dllexport) __stdcall void sv_Init()
{
    sys_InitSubsystems(SYS_SUBSYSTEM_PHYSICS | SYS_SUBSYSTEM_ENTITY | SYS_SUBSYSTEM_PLAYER);
}

__declspec(dllexport) __stdcall void sv_Shutdown()
{

}

__declspec(dllexport) __stdcall void sv_Step()
{
    phy_Step();
    ent_UpdateTransformComponents();
}

__declspec(dllexport) __stdcall void sv_ChangeLevel(char *level_name)
{

}

#ifdef __cplusplus
}
#endif
