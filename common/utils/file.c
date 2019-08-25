#include "file.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


long file_FileSize(FILE *file)
{
    long size;
    long offset;

    offset = ftell(file);
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, offset, SEEK_SET);

    return size;
}

void file_ReadFile(FILE *file, void **buffer, long *buffer_size)
{
    char *file_buffer = NULL;
    long file_size = 0;

    if(file)
    {
        file_size = file_FileSize(file);
        file_buffer = (char *)calloc(file_size + 1, 1);
        fread(file_buffer, file_size, 1, file);
        file_buffer[file_size] = '\0';
    }

    *buffer = (void *)file_buffer;
    *buffer_size = file_size;
}

void file_WriteFile(void **buffer, long *buffer_size)
{

}

int file_FileExists(char *file_name)
{
    FILE *file;

    file = fopen(file_name, "r");

    if(file)
    {
        fclose(file);
        return 1;
    }

    return 0;
}




