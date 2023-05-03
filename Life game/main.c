#include <stdio.h>
#include <malloc.h>
#include <string.h>

int counter_for_neighbours(int **area, int x, int y)
{
    int sum = 0;
    for (int i = x - 1; i <= x + 1; i++)
    {
        for (int j = y - 1; j <= y + 1; j++)
        {
            sum += area[i][j];
        }
    }
    if (area[x][y] == 1)
    {
        sum--;
    }
    return sum;
}

unsigned char *convert_to_str(int **arr, unsigned long height, unsigned long width)
{
    unsigned char *info_on_current_pixel = (unsigned char *) malloc(height * width * 3);
    int temp_index = 0;

    for (int i = height - 1; i >= 0; i--)
    {
        for (int j = 0; j < width; j++)
        {
            if (arr[i][j] == 1)
            {
                info_on_current_pixel[temp_index] = 0;
                info_on_current_pixel[temp_index + 1] = 0;
                info_on_current_pixel[temp_index + 2] = 0;
            }
            else
            {
                info_on_current_pixel[temp_index] = 255;
                info_on_current_pixel[temp_index + 1] = 255;
                info_on_current_pixel[temp_index + 2] = 255;
            }
            temp_index += 3;
        }
    }

    return info_on_current_pixel;
}

int main(int argc, char *argv[])
{
    FILE *image_file;

    long dump_freq = 1;
    long max_iter = 1;
    char *dir_name;

    unsigned long height;
    unsigned long width;
    unsigned long size;
    unsigned long image_offset;
    unsigned char bmp_header[54];

    int **current_generation;
    int **next_generation;

    for (int i = 1; i < argc; i += 2)
    {
        if (strcmp(argv[i], "--input") == 0)
        {
            image_file = fopen(argv[i + 1], "rb");  // rb - считывание в бинарном виде
            if (image_file == NULL)
            {
                printf("An error occurred while opening the image file");
                return 0;
            }
        }
        else if (strcmp(argv[i], "--output") == 0)
        {
            dir_name = argv[i + 1];
        }
        else if (strcmp(argv[i], "--max_iter") == 0)
        {
            max_iter = strtol(argv[i + 1], NULL, 10);
        }
        else if (strcmp(argv[i], "--dump_freq") == 0)
        {
            dump_freq = strtol(argv[i + 1], NULL, 10);
        }
    }

    fread(bmp_header, sizeof (unsigned char ), 54, image_file);

    image_offset = bmp_header[0xD] << 24 | bmp_header[0xC] << 16 | bmp_header[0xB] << 8 | bmp_header[0xA];
    size = bmp_header[0x5] << 24 | bmp_header[0x4] << 16 | bmp_header[0x3] << 8 | bmp_header[0x2];
    width = bmp_header[0x15] << 24 | bmp_header[0x14] << 16 | bmp_header[0x13] << 8 | bmp_header[0x12];
    height = bmp_header[0x19] << 24 | bmp_header[0x18] << 16 | bmp_header[0x17] << 8 | bmp_header[0x16];

    printf("offset - %lu\n", image_offset);
    printf("size - %lu\n", size);
    printf("height - %lu\n", height);
    printf("width - %lu\n", width);
    printf("\n");

    current_generation = (int **) malloc(height * sizeof (int *));
    for (int i = 0; i < height; i++)
    {
        current_generation[i] = (int *) malloc(width * 3 * sizeof (int *));
    }

    next_generation = (int **) malloc(height * sizeof (int *));
    for (int i = 0; i < height; i++)
    {
        next_generation[i] = (int *) malloc(width * 3 * sizeof (int *));
    }

    fseek(image_file, image_offset, SEEK_SET);

    char buffer[3];
    for (int i = height - 1; i >= 0; i--)
    {
        for (int j = 0; j < width; j++)
        {
            buffer[0] = fgetc(image_file);
            buffer[1] = fgetc(image_file);
            buffer[2] = fgetc(image_file);

            if (buffer[0] != 0 || buffer[1] != 0 || buffer[2] != 0)
            {
                current_generation[i][j] = 1;
            }
            else
            {
                current_generation[i][j] = 0;
            }
        }
    }

    char file_name[10];
    char directory[256];
    char *pixel_info;

    for (unsigned long i = 0; i < height; i++)
    {
        for (unsigned long j = 0; j < width; j++)
        {
            next_generation[i][j] = current_generation[i][j];
        }
    }

    int number_of_neighbours;
    for (int game_iteration = 0; game_iteration < max_iter; game_iteration++)
    {
        for (unsigned long i = 1; i < height - 1; i++)  // Изменения следующего поколения
        {
            for (unsigned long j = 1; j < width - 1; j++)
            {
                number_of_neighbours = counter_for_neighbours(current_generation, i, j);
                if (current_generation[i][j] == 0 && number_of_neighbours == 3)
                {
                    next_generation[i][j] = 1;
                }
                else if (current_generation[i][j] == 1)
                {
                    if (number_of_neighbours < 2 || number_of_neighbours > 3)
                    {
                        next_generation[i][j] = 0;
                    }
                }
            }
        }

        for (unsigned long i = 0; i < height; i++)
        {
            for (unsigned long j = 0; j < width; j++)
            {
                current_generation[i][j] = next_generation[i][j];
            }
        }

        pixel_info = convert_to_str(current_generation, height, width);

        if (game_iteration % dump_freq == 0)
        {
            sprintf(file_name, "%d", game_iteration);
            strcpy(directory, dir_name);
            strcat(directory, "/");
            strcat(directory, file_name);
            strcat(directory, ".bmp");

            FILE *new_bmp_file = fopen(directory, "wb");

            if (new_bmp_file != NULL)
            {
                printf("The file %d was created\n", game_iteration);
            }
            else
            {
                printf("An error occurred while opening file %4d\n", game_iteration);
            }
            fseek(new_bmp_file, 0, SEEK_SET);
            fwrite(bmp_header, 1, 54, new_bmp_file);
            fwrite(pixel_info, 1, 3 * height * width, new_bmp_file);
        }
    }

    free(current_generation);
    free(next_generation);
    free(pixel_info);

    return 0;
}