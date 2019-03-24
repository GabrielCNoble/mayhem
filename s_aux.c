#include "s_aux.h"
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

unsigned long aux_FileSize(FILE *file)
{
    unsigned long size = 0;
    unsigned long cur = 0;

    if(file)
    {
        cur = ftell(file);
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, cur, SEEK_SET);
    }

    return size;
}

void *aux_ReadFile(FILE *file)
{
    void *file_buffer = NULL;
    unsigned long file_size;

    if(file)
    {
        file_size = aux_FileSize(file);
        file_buffer = calloc(file_size, 1);
        fread(file_buffer, file_size, 1, file);
    }

    return file_buffer;
}

void aux_LoadWavefront(char *file_name,  struct geometry_data_t *geometry_data)
{
    geometry_data->vertices.init(sizeof(vec3_t), 32);
    geometry_data->normals.init(sizeof(vec3_t), 32);
    geometry_data->tangents.init(sizeof(vec3_t), 32);
    geometry_data->tex_coords.init(sizeof(vec2_t), 32);

    struct face_t
    {
        struct list_t vertices;
    };

    struct face_vertice_t
    {
        int indices[3];
    };

    struct list_t *attrib_list;

    struct list_t faces(sizeof(struct face_t), 32);
    struct list_t vertices(sizeof(vec3_t), 32);
    struct list_t normals(sizeof(vec3_t), 32);
    struct list_t tex_coords(sizeof(vec3_t), 32);

    struct face_t *face;
    struct face_vertice_t face_vertice;

    int i = 0;
    int j;
    char *file_buffer;
    unsigned long file_size;
    vec4_t vec_attrib;
    int attrib_size;
    FILE *file;

    int value_string_index;
    char value_string[64];

    file = fopen(file_name, "rb");

    if(file)
    {
        file_size = aux_FileSize(file);
        file_buffer = (char *)aux_ReadFile(file);
        fclose(file);

        while(i < file_size)
        {
            switch(file_buffer[i])
            {
                case 'v':
                    i++;

                    if(file_buffer[i] == 't')
                    {
                        attrib_list = &tex_coords;
                        attrib_size = 2;
                        i++;
                    }
                    else
                    {
                        attrib_size = 3;
                        if(file_buffer[i] == 'n')
                        {
                            attrib_list = &normals;
                            i++;
                        }
                        else
                        {
                            attrib_list = &vertices;
                        }
                    }

                    for(j = 0; j < attrib_size; j++)
                    {
                        while(file_buffer[i] == ' ')i++;

                        value_string_index = 0;

                        while((file_buffer[i] >= '0' && file_buffer[i] <= '9') || file_buffer[i] == '.' || file_buffer[i] == '-')
                        {
                            value_string[value_string_index] = file_buffer[i];
                            value_string_index++;
                            i++;
                        }

                        value_string[value_string_index] = '\0';

                        vec_attrib[j] = atof(value_string);
                    }

                    attrib_list->add(&vec_attrib);
                break;

                case 'f':


                    i++;

                    face = (struct face_t *)faces.get(faces.add(NULL));

                    face->vertices.init(sizeof(struct face_vertice_t), 3);

                    while(file_buffer[i] != '\0' && file_buffer[i] != '\n' &&
                          file_buffer[i] != '\r')

                    {
                        for(j = 0; j < 3; j++)
                        {
                            while(file_buffer[i] == ' ')i++;

                            value_string_index = 0;

                            while(file_buffer[i] >= '0' && file_buffer[i] <= '9')
                            {
                                value_string[value_string_index] = file_buffer[i];
                                value_string_index++;
                                i++;
                            }

                            value_string[value_string_index] = '\0';

                            if(file_buffer[i] == '/')
                            {
                                i++;
                            }

                            if(value_string_index)
                            {
                                face_vertice.indices[j] = atoi(value_string) - 1;
                            }
                            else
                            {
                                face_vertice.indices[j] = -1;
                            }
                        }

                        face->vertices.add(&face_vertice);

                        while(file_buffer[i] == ' ')i++;
                    }
                break;

                case '#':
                case 's':
                    while(file_buffer[i] != '\0' && file_buffer[i] != '\n' && file_buffer[i] != '\r') i++;
                break;

                default:
                    i++;
                break;
            }
        }


        for(i = 0; i < faces.cursor; i++)
        {
            face = (struct face_t *)faces.get(i);

            for(j = 0; j < face->vertices.cursor; j++)
            {
                face_vertice = *(struct face_vertice_t *)face->vertices.get(j);

                geometry_data->vertices.add(vertices.get(face_vertice.indices[0]));
                geometry_data->normals.add(normals.get(face_vertice.indices[2]));

                if(face_vertice.indices[1] != -1)
                {
                    geometry_data->tex_coords.add(tex_coords.get(face_vertice.indices[1]));
                }
                else
                {
                    //geometry_data->tex_coords.add();
                }
            }
        }
    }
}



#ifdef __cplusplus
}
#endif






