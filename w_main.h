#ifndef W_MAIN_H
#define W_MAIN_H



#ifdef __cplusplus
extern "C"
{
#endif


void w_Init();

void w_Shutdown();

void w_LoadLevel(char *file_name);

void w_LoadLevelFromMemory(void *buffer);

#ifdef __cplusplus
}
#endif

#endif // W_MAIN_H
