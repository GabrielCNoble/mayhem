#include "gmy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void gmy_LoadGmy(char *file_name, struct geometry_data_t *geometry_data)
{
    FILE *file;
    void *file_buffer;
    unsigned int file_size;
    unsigned char *in;

    geometry_data->batches.init(sizeof(struct batch_data_t), 32);
    geometry_data->materials.init(sizeof(struct material_data_t), 32);
    geometry_data->vertices.init(sizeof(vec3_t), 32);
    geometry_data->normals.init(sizeof(vec3_t), 32);
    geometry_data->tangents.init(sizeof(vec3_t), 32);
    geometry_data->tex_coords.init(sizeof(vec2_t), 32);

    file = fopen(file_name, "rb");

    if(file)
    {

    }
}

void gmy_SaveGmy(char *file_name, struct geometry_data_t *geometry_data)
{
    unsigned int file_size;
    void *file_buffer;
    FILE *file;
    unsigned char *out;

    struct gmy_start_t *start;
    struct gmy_end_t *end;
    struct gmy_batch_t *gmy_batch;
    struct batch_data_t *batches;
    struct material_data_t *materials;
    char output_name[512];

    int i;
    int j;


    file_size = sizeof(struct gmy_start_t) + sizeof(struct gmy_end_t);
    file_size += sizeof(struct gmy_batch_t) * geometry_data->batches.cursor;
    file_size += sizeof(vec3_t) * geometry_data->vertices.cursor;
    file_size += sizeof(vec3_t) * geometry_data->normals.cursor;
    file_size += sizeof(vec3_t) * geometry_data->tangents.cursor;
    file_size += sizeof(vec2_t) * geometry_data->tex_coords.cursor;


    file_buffer = (void *)calloc(file_size, 1);

    out = (unsigned char *)file_buffer;


    start = (struct gmy_start_t *)out;
    out += sizeof(struct gmy_start_t);

    strcpy(start->tag, gmy_start_tag);
    start->batch_count = geometry_data->batches.cursor;
    start->vertice_count = geometry_data->vertices.cursor;

    batches = (struct batch_data_t *)geometry_data->batches.buffer;
    materials = (struct material_data_t *)geometry_data->materials.buffer;

    for(i = 0; i < start->batch_count; i++)
    {
        gmy_batch = (struct gmy_batch_t *)out;
        out += sizeof(struct gmy_batch_t);

        for(j = 0; j < geometry_data->batches.cursor; j++)
        {
            if(!strcmp(batches[i].material, materials[j].name))
            {
                break;
            }
        }

        strcpy(gmy_batch->tag, gmy_batch_tag);
        strcpy(gmy_batch->material, materials[j].name);
        strcpy(gmy_batch->diffuse_texture, materials[j].diffuse_texture);
        strcpy(gmy_batch->normal_texture, materials[j].normal_texture);
        gmy_batch->base = materials[j].base;


        memcpy(out, geometry_data->vertices.get(batches[i].start), sizeof(vec3_t) * batches[i].count);
        out += sizeof(vec3_t) * batches[i].count;

        if(geometry_data->normals.cursor)
        {
            memcpy(out, geometry_data->normals.get(batches[i].start), sizeof(vec3_t) * batches[i].count);
        }
        out += sizeof(vec3_t) * batches[i].count;

        if(geometry_data->tangents.cursor)
        {
            memcpy(out, geometry_data->tangents.get(batches[i].start), sizeof(vec3_t) * batches[i].count);
        }
        out += sizeof(vec3_t) * batches[i].count;

        if(geometry_data->tex_coords.cursor)
        {
            memcpy(out, geometry_data->tex_coords.get(batches[i].start), sizeof(vec3_t) * batches[i].count);
        }
        out += sizeof(vec2_t) * batches[i].count;
    }


    end = (struct gmy_end_t *)out;
    out += sizeof(struct gmy_end_t);
    strcpy(end->tag, gmy_end_tag);

    for(i = strlen(file_name); i > 0; i--)
    {
        if(file_name[i] == '.')
        {
            break;
        }
    }

    if(!i)
    {
        i = strlen(file_name);
    }

    output_name[i] = '\0';
    i--;

    for(; i >= 0; i--)
    {
        output_name[i] = file_name[i];
    }

    strcat(output_name, ".gmy");

    file = fopen(output_name, "wb");
    fwrite(file_buffer, file_size, 1, file);
    fclose(file);
}




