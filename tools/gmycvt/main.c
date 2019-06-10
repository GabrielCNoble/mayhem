#include "main.h"



int main(int argc, char *argv[])
{
    int i;

    char *input_file = NULL;
    char *output_file = NULL;
    struct geometry_data_t geometry_data;
    FILE *file;

    if(argc > 1)
    {
        i = 1;

        while(i < argc)
        {
            if(!strcmp(argv[i], "-f"))
            {
                if(input_file)
                {
                    printf("error: input file already specified!\n");
                    break;
                }

                i++;

                if(i >= argc)
                {
                    printf("error: missing input file name!\n");
                    return -1;
                }

                input_file = argv[i];

                i++;
            }
            else if(!strcmp(argv[i], "-o"))
            {
                if(output_file)
                {
                    printf("error: output file already specified!\n");
                    return -1;
                }

                i++;

                if(i >= argc)
                {
                    break;
                }

                output_file = argv[i];

                i++;
            }
            else
            {
                i++;
            }
        }

        if(!output_file)
        {
            output_file = input_file;
        }

        file = fopen(input_file, "rb");

        if(!file)
        {
            printf("error: input file %s doesn't exist!\n", input_file);
            return -1;
        }

        fclose(file);

        obj_LoadWavefront(input_file, &geometry_data);
        gmy_SaveGmy(output_file, &geometry_data);

        struct geometry_data_t loaded;
        gmy_LoadGmy(output_file, &loaded);

        printf("vertices: %d\n", loaded.vertices.cursor);
        printf("normals: %s\n", loaded.normals.cursor ? "yes" : "no");
        printf("tangets: %s\n", loaded.tangents.cursor ? "yes" : "no");
        printf("tex coords: %s\n", loaded.tex_coords.cursor ? "yes" : "no");
        printf("batches: %d\n", loaded.batches.cursor);
    }
}




