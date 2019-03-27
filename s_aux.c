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
    geometry_data->draw_batches.init(sizeof(struct draw_batch_t), 32);

    struct face_t
    {
        struct list_t vertices;
        short material;
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

    struct draw_batch_t *batch;

    struct face_t *face;
    struct face_vertice_t face_vertice;

    int i = 0;
    int j = 0;
    int k = 0;
    char *file_buffer;
    unsigned long file_size;
    vec4_t vec_attrib;
    int attrib_size;
    FILE *file;

    short current_material;

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

                    face->material = current_material;

                    for(j = 0; j < geometry_data->draw_batches.cursor; j++)
                    {
                        batch = (struct draw_batch_t *)geometry_data->draw_batches.get(j);

                        if(batch->material == current_material)
                        {
                            break;
                        }
                    }

                    if(j >= geometry_data->draw_batches.cursor)
                    {
                        batch = (struct draw_batch_t *)geometry_data->draw_batches.get(geometry_data->draw_batches.add(NULL));
                        batch->material = current_material;
                    }

                    while(file_buffer[i] != '\0' && file_buffer[i] != '\n' &&
                          file_buffer[i] != '\r' && file_buffer[i] != '\t')

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

                case 'u':

                    i++;

                    if(file_buffer[i    ] == 's' &&
                       file_buffer[i + 1] == 'e' &&
                       file_buffer[i + 2] == 'm' &&
                       file_buffer[i + 3] == 't' &&
                       file_buffer[i + 4] == 'l' &&
                       file_buffer[i + 5] == ' ')
                    {
                        i += 6;

                        while(file_buffer[i] == ' ') i++;

                        value_string_index = 0;

                        while(file_buffer[i] != ' ' &&
                              file_buffer[i] != '\n' &&
                              file_buffer[i] != '\r' &&
                              file_buffer[i] != '\0' &&
                              file_buffer[i] != '\t')
                        {
                            value_string[value_string_index] = file_buffer[i];
                            value_string_index++;
                            i++;
                        }

                        value_string[value_string_index] = '\0';

                        current_material = r_GetMaterialHandle(value_string);
                    }
                    else
                    {
                        i++;
                    }
                break;

                case 'm':
                    i++;

                    if(file_buffer[i    ] == 't' &&
                       file_buffer[i + 1] == 'l' &&
                       file_buffer[i + 2] == 'l' &&
                       file_buffer[i + 3] == 'i' &&
                       file_buffer[i + 4] == 'b' &&
                       file_buffer[i + 5] == ' ')
                    {
                        i += 6;

                        while(file_buffer[i] == ' ') i++;

                        value_string_index = 0;

                        while(file_buffer[i] != ' ' &&
                              file_buffer[i] != '\n' &&
                              file_buffer[i] != '\r' &&
                              file_buffer[i] != '\0' &&
                              file_buffer[i] != '\t')
                        {
                            value_string[value_string_index] = file_buffer[i];
                            value_string_index++;
                            i++;
                        }

                        value_string[value_string_index] = '\0';

                        aux_LoadWavefrontMtl(value_string, geometry_data);
                    }
                    else
                    {
                        i++;
                    }
                break;

                default:
                    i++;
                break;
            }
        }

        for(k = 0; k < geometry_data->draw_batches.cursor; k++)
        {
            batch = (struct draw_batch_t *)geometry_data->draw_batches.get(k);

            if(k)
            {
                batch->start = (batch - 1)->start + (batch - 1)->count;
            }

            for(i = 0; i < faces.cursor; i++)
            {
                face = (struct face_t *)faces.get(i);

                if(face->material == batch->material)
                {
                    for(j = 0; j < face->vertices.cursor; j++)
                    {
                        face_vertice = *(struct face_vertice_t *)face->vertices.get(j);

                        geometry_data->vertices.add(vertices.get(face_vertice.indices[0]));
                        geometry_data->normals.add(normals.get(face_vertice.indices[2]));

                        if(face_vertice.indices[1] != -1)
                        {
                            geometry_data->tex_coords.add(tex_coords.get(face_vertice.indices[1]));
                        }
                    }

                    batch->count += face->vertices.cursor;
                }
            }
        }
    }
}


void aux_LoadWavefrontMtl(char *file_name, struct geometry_data_t *geometry_data)
{
    int i = 0;
    int j;
    char *file_buffer;
    unsigned long file_size;
    struct material_t *current_material = NULL;
    int value_str_index;
    unsigned short material_handle;
    char value_str[64];
    vec4_t color;
    FILE *file;

    file = fopen(file_name, "rb");

    if(file)
    {
        file_buffer = (char *)aux_ReadFile(file);
        file_size = aux_FileSize(file);
        fclose(file);

        while(i < file_size)
        {
            switch(file_buffer[i])
            {
                case 'K':
                    i++;

                    if(file_buffer[i] == 'a' || file_buffer[i] == 's')
                    {
                        while(file_buffer[i] != '\n' && file_buffer[i] != '\r' && file_buffer[i] != '\0')i++;
                    }
                    else if(file_buffer[i] == 'd')
                    {
                        for(j = 0; j < 3; j++)
                        {
                            value_str_index = 0;

                            while(file_buffer[i] == ' ') i++;

                            while(file_buffer[i] != ' ' &&
                                  file_buffer[i] != '\n' &&
                                  file_buffer[i] != '\r' &&
                                  file_buffer[i] != '\0' &&
                                  file_buffer[i] != '\t')
                            {
                                value_str[value_str_index] = file_buffer[i];
                                i++;
                                value_str_index++;
                            }

                            color[j] = atof(value_str);
                        }

                        color[3] = 1.0;

                        r_SetMaterialColorPointer(current_material, color);
                    }
                break;

                case 'n':
                    i++;

                    if(file_buffer[i] == 'e' &&
                       file_buffer[i + 1] == 'w' &&
                       file_buffer[i + 2] == 'm' &&
                       file_buffer[i + 3] == 't' &&
                       file_buffer[i + 4] == 'l' &&
                       file_buffer[i + 5] == ' ')
                    {
                        i += 6;

                        while(file_buffer[i] == ' ')i++;

                        value_str_index = 0;

                        while(file_buffer[i] != ' ' && file_buffer[i] != '\n' &&
                              file_buffer[i] != '\r' && file_buffer[i] != '\0' &&
                              file_buffer[i] != '\t')
                        {
                            value_str[value_str_index] = file_buffer[i];
                            value_str_index++;
                            i++;
                        }

                        value_str[value_str_index] = '\0';

                        material_handle = r_CreateEmptyMaterial();
                        current_material = r_GetMaterialPointer(material_handle);

                        current_material->name = strdup(value_str);
                    }
                    else
                    {
                        i++;
                    }
                break;

                case 'd':
                case '#':
                    while(file_buffer[i] != '\n' && file_buffer[i] != '\r' && file_buffer[i] != '\0' && file_buffer[i] != '\t')i++;
                break;

                case 'i':
                    if(!strcmp(file_buffer + i, "illum"))
                    {
                        while(file_buffer[i] != '\n' && file_buffer[i] != '\r' && file_buffer[i] != '\0' && file_buffer[i] != '\t')i++;
                    }
                    else
                    {
                        i++;
                    }
                break;

                default:
                    i++;
                break;
            }
        }

    }

}



#ifdef __cplusplus
}
#endif






