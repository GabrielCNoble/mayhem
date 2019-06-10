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
    struct gmy_start_t *start;
    struct gmy_batch_t *gmy_batch;
//    struct material_data_t material;
    struct batch_data_t batch;
    int i;

    geometry_data->batches.init(sizeof(struct batch_data_t), 32);
//    geometry_data->materials.init(sizeof(struct material_data_t), 32);
    geometry_data->vertices.init(sizeof(vec3_t), 32);
    geometry_data->normals.init(sizeof(vec3_t), 32);
    geometry_data->tangents.init(sizeof(vec3_t), 32);
    geometry_data->tex_coords.init(sizeof(vec2_t), 32);

    file = fopen(file_name, "rb");

    if(file)
    {
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);

        file_buffer = (void *)calloc(file_size, 1);

        fread(file_buffer, file_size, 1, file);
        fclose(file);

        in = (unsigned char *)file_buffer;

        start = (struct gmy_start_t *)in;
        in += sizeof(struct gmy_start_t);

        for(i = 0; i < start->batch_count; i++)
        {
            gmy_batch = (struct gmy_batch_t *)in;
            in += sizeof(struct gmy_batch_t);
            geometry_data->batches.add(&gmy_batch->batch);
        }

        /* no need to check whether there are normals, tangents or tex coords here.
        They'll always be present, even if being zero vectors... */

        /* vertices... */
        geometry_data->vertices.add_multiple(in, start->vertice_count);
        in += sizeof(vec3_t) * start->vertice_count;

        /* normals... */
        geometry_data->normals.add_multiple(in, start->vertice_count);
        in += sizeof(vec3_t) * start->vertice_count;

        /* tangents... */
        geometry_data->tangents.add_multiple(in, start->vertice_count);
        in += sizeof(vec3_t) * start->vertice_count;

        /* tex coords... */
        geometry_data->tex_coords.add_multiple(in, start->vertice_count);
        in += sizeof(vec2_t) * start->vertice_count;

        end = (struct gmy_end_t *)in;
        in += sizeof(struct gmy_end_t );

        free(file_buffer);
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
//    struct material_data_t *materials;
    char output_name[512];

    int i;
    int j;

    file_size = sizeof(struct gmy_start_t) + sizeof(struct gmy_end_t);
    file_size += sizeof(struct gmy_batch_t) * geometry_data->batches.cursor;
    file_size += ((sizeof(vec3_t) * 3) + sizeof(vec2_t)) * geometry_data->vertices.cursor;

    file_buffer = (void *)calloc(file_size, 1);
    out = (unsigned char *)file_buffer;

    start = (struct gmy_start_t *)out;
    out += sizeof(struct gmy_start_t);

    strcpy(start->tag, gmy_start_tag);
    start->batch_count = geometry_data->batches.cursor;
    start->vertice_count = geometry_data->vertices.cursor;

    batches = (struct batch_data_t *)geometry_data->batches.buffer;

    for(i = 0; i < start->batch_count; i++)
    {
        gmy_batch = (struct gmy_batch_t *)out;
        out += sizeof(struct gmy_batch_t);

        strcpy(gmy_batch->tag, gmy_batch_tag);

        /* gmy uses the exchange batch_data_t structure to store batch data... */
        memcpy(&gmy_batch->batch, batches + i, sizeof(struct batch_data_t));
    }

    memcpy(out, geometry_data->vertices.buffer, sizeof(vec3_t) * geometry_data->vertices.cursor);
    out += sizeof(vec3_t) * geometry_data->vertices.cursor;

    /* the output file will be filled with zero bytes if there are no normals... */
    if(geometry_data->normals.cursor)
    {
        memcpy(out, geometry_data->normals.buffer, sizeof(vec3_t) * geometry_data->normals.cursor);
    }
    out += sizeof(vec3_t) * geometry_data->normals.cursor;


    /* the output file will be filled with zero bytes if there are no tangents... */
    if(geometry_data->tangents.cursor)
    {
        memcpy(out, geometry_data->tangents.buffer, sizeof(vec3_t) * geometry_data->tangents.cursor);
    }
    out += sizeof(vec3_t) * geometry_data->tangents.cursor;


    /* the output file will be filled with zero bytes if there are no tex coords... */
    if(geometry_data->tex_coords.cursor)
    {
        memcpy(out, geometry_data->tex_coords.buffer, sizeof(vec3_t) * geometry_data->tangents.cursor);
    }
    out += sizeof(vec2_t) * geometry_data->tangents.cursor;


    end = (struct gmy_end_t *)out;
    out += sizeof(struct gmy_end_t);
    strcpy(end->tag, gmy_end_tag);


    /* make sure the output file name has the ".gmy" ext... */
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




