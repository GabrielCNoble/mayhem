#include "path.h"
#include <string.h>
#include <limits.h>



char *path_GetFilePath(char *file_name)
{
    static char path[PATH_MAX];
    int index;
    int len;

    len = strlen(file_name);

    file_name = path_FormatPath(file_name);

    for(index = len; index >= 0; index--)
    {
        if(file_name[index] == '/')
        {
            break;
        }
    }

    if(!index)
    {
        strcpy(path, "./");
    }
    else
    {
        path[index] = '\0';

        for(index--; index >= 0; index--)
        {
            path[index] = file_name[index];
        }
    }

    return path;
}


char *path_FormatPath(char *path)
{
    static char formatted_path[PATH_MAX];
    int i;
    int j;

    memset(formatted_path, 0, PATH_MAX);

    for(i = 0, j = 0; path[i]; i++)
    {
        formatted_path[j] = path[i];

        if(path[i] == '\\')
        {
            formatted_path[j] = '/';

            if(path[i + 1] == '\\')
            {
                i++;
            }
        }

        j++;
    }

    formatted_path[j] = '\0';

    return formatted_path;
}

char *path_AppendPathSegment(char *path, char *segment)
{
    static char new_path[PATH_MAX];

    strcpy(new_path, path);
    strcat(new_path, "/");
    strcat(new_path, path_FormatPath(segment));

    return new_path;
}
