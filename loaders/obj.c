
#include "obj.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct face_t
{
    struct list_t vertices;
    char material[64];
};

struct face_vertice_t
{
    int indices[3];
};


void obj_LoadWavefront(char *file_name,  struct geometry_data_t *geometry_data)
{
    geometry_data->vertices.init(sizeof(vec3_t), 32);
    geometry_data->normals.init(sizeof(vec3_t), 32);
    geometry_data->tangents.init(sizeof(vec3_t), 32);
    geometry_data->tex_coords.init(sizeof(vec2_t), 32);
    geometry_data->batches.init(sizeof(struct batch_data_t), 32);
    geometry_data->materials.init(sizeof(struct material_data_t), 32);



    struct list_t *attrib_list;

    struct list_t faces(sizeof(struct face_t), 32);
    struct list_t vertices(sizeof(vec3_t), 32);
    struct list_t normals(sizeof(vec3_t), 32);
    struct list_t tex_coords(sizeof(vec3_t), 32);

    struct batch_data_t *batch;

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

//    short current_material;
    struct material_data_t *current_material;
    int value_string_index;
    char value_string[128];

    file = fopen(file_name, "rb");

    if(file)
    {
//        file_size = aux_FileSize(file);
//        file_buffer = (char *)aux_ReadFile(file);
//        fclose(file);

        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);
        file_buffer = (char *)calloc(file_size + 1, 1);
        fread(file_buffer, file_size, 1, file);
        fclose(file);
        file_buffer[file_size] = '\0';

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

                    /* new face starts... */
                    face = (struct face_t *)faces.get(faces.add(NULL));
                    face->vertices.init(sizeof(struct face_vertice_t), 3);
                    strcpy(face->material, current_material->name);

                    for(j = 0; j < geometry_data->batches.cursor; j++)
                    {
                        /* try to find a batch that has this material... */
                        batch = (struct batch_data_t *)geometry_data->batches.get(j);

                        if(!strcmp(batch->material, current_material->name))
                        {
                            break;
                        }
                    }

                    if(j >= geometry_data->batches.cursor)
                    {
                        /* no batch using the current material exists, so create a new one... */
                        batch = (struct batch_data_t *)geometry_data->batches.get(geometry_data->batches.add(NULL));
                        strcpy(batch->material, current_material->name);
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

//                        current_material = r_GetMaterialHandle(value_string);
                        current_material = obj_GetMaterial(value_string, geometry_data);
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

                        obj_LoadWavefrontMtl(value_string, geometry_data);
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

        for(k = 0; k < geometry_data->batches.cursor; k++)
        {
            batch = (struct batch_data_t *)geometry_data->batches.get(k);

            if(k)
            {
                batch->start = (batch - 1)->start + (batch - 1)->count;
            }

            for(i = 0; i < faces.cursor; i++)
            {
                face = (struct face_t *)faces.get(i);

                if(!strcmp(face->material, batch->material))
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


void obj_LoadWavefrontMtl(char *file_name, struct geometry_data_t *geometry_data)
{
    int i = 0;
    int j;
    char *file_buffer;
    unsigned long file_size;
    struct material_data_t *current_material = NULL;
    int value_str_index;
    unsigned short material_handle;
    char value_str[64];
    vec4_t color;
    FILE *file;

    file = fopen(file_name, "rb");

    if(file)
    {
//        file_buffer = (char *)aux_ReadFile(file);
//        file_size = aux_FileSize(file);
//        fclose(file);

        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);
        file_buffer = (char *)calloc(file_size + 1, 1);
        fread(file_buffer, file_size, 1, file);
        fclose(file);
        file_buffer[file_size] = '\0';

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
                        i++;

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

                            value_str[value_str_index] = '\0';

                            color[j] = atof(value_str);
                        }

                        color[3] = 1.0;

                        current_material->base = color;

//                        r_SetMaterialColorPointer(current_material, color);
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

                        current_material = (struct material_data_t *)geometry_data->materials.get(geometry_data->materials.add(NULL));
                        strcpy(current_material->name, value_str);

//                        material_handle = r_CreateEmptyMaterial();
//                        current_material = r_GetMaterialPointer(material_handle);
//
//                        current_material->name = strdup(value_str);
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


struct material_data_t *obj_GetMaterial(char *material_name, struct geometry_data_t *geometry_data)
{
    struct material_data_t *materials;
    int i;
    int c;

    materials = (struct material_data_t *)geometry_data->materials.buffer;
    c = geometry_data->materials.cursor;

    for(i = 0; i < c; i++)
    {
        if(!strcmp(material_name, materials[i].name))
        {
            return materials + i;
        }
    }

    for(i = 0; i < c; i++)
    {
        if(!strcmp(DEFAULT_MATERIAL_NAME, materials[i].name))
        {
            return materials + i;
        }
    }

    struct material_data_t default_material = DEFAULT_MATERIAL;
    i = geometry_data->materials.add(&default_material);
    materials = (struct material_data_t *)geometry_data->materials.get(i);

    return materials;
}










