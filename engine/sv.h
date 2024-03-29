#ifndef SV_H
#define SV_H

#ifdef __cplusplus
extern "C"
{
#endif

void sv_Init();

void sv_Shutdown();

void sv_Step();

void sv_ChangeLevel(char *level_name);

void sv_LoadLevelCollision();

#ifdef __cplusplus
}
#endif

#endif // SV_H
