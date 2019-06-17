#ifndef FILE_H
#define FILE_H


#include <stdio.h>


long file_GetFileSize(FILE *file);

void file_ReadFile(FILE *file, void **buffer, long *buffer_size);

int file_FileExists(char *file_name);


#endif // FILE_H
