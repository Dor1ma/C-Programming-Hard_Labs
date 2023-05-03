#include <stdio.h>
#include <string.h>
#include <malloc.h>
#define ID3_max_size 128

typedef struct ID3_tag
{
    char tag[3],
    song_name[30],
    artist[20],
    album[30],
    year[4];
} ID3_tag;

long file_offset(FILE *original_file)
{
    fseek(original_file, 0, SEEK_END);
    return ftell(original_file) - ID3_max_size;
}

void show(ID3_tag *tags_storage)
{
    printf("Song name: %s\n", tags_storage -> song_name);
    printf("Artist: %s\n", tags_storage -> artist);
    printf("Album: %s\n", tags_storage -> album);
    printf("Year: %s\n", tags_storage -> year);
}

void get(char *tag, ID3_tag *tags_storage)
{
    if (strcmp(tag, "song_name") == 0)
    {
        printf("Song name: %s\n", tags_storage -> song_name);
    }
    else if (strcmp(tag, "artist") == 0)
    {
        printf("Artist: %s\n", tags_storage -> artist);
    }
    else if (strcmp(tag, "album") == 0)
    {
        printf("Album: %s\n", tags_storage -> album);
    }
    else if (strcmp(tag, "year") == 0)
    {
        printf("Year: %s\n", tags_storage -> year);
    }
    else
    {
        printf("Unknown tag\n");
    }
}

void set(char *tag_to_change, char *value_to_change, ID3_tag *tags_storage)
{
    if (strcmp(tag_to_change, "song_name") == 0)
    {
        memcpy(tags_storage -> song_name, value_to_change, 30);
    }
    else if (strcmp(tag_to_change, "artist") == 0)
    {
        memcpy(tags_storage -> artist, value_to_change, 20);
    }
    else if (strcmp(tag_to_change, "album") == 0)
    {
        memcpy(tags_storage -> album, value_to_change, 20);
    }
    else if (strcmp(tag_to_change, "year") == 0)
    {
        memcpy(tags_storage -> year, value_to_change, 4);
    }
    else
    {
        printf("Unknown parameter of command '--value='\n");
    }
}

int main(int argc, char *argv[]) {

    FILE *original_file = NULL;
    FILE *changed_file = NULL;

    char *mp3_name;
    char *tag_to_change;
    char *value_to_change;

    ID3_tag *tags = NULL;

    const int fp_elements_amount = 11;

    if (strncmp(argv[1], "--filepath=", fp_elements_amount) == 0)
    {
        mp3_name = strpbrk(argv[1], "=") + 1;
        if ((original_file = fopen(mp3_name, "rb")) == NULL)
        {
            printf("An error occurred while opening file %s\n", mp3_name);
            return 0;
        }
    }
    else
    {
        printf("The command '--filepath=' is not recognised\n");
        return 0;
    }

    char *temp = (char *) malloc(sizeof (char ) * ID3_max_size);
    memset(temp, 0, ID3_max_size);

    fseek(original_file, file_offset(original_file), SEEK_SET);
    fread(temp, 1, ID3_max_size, original_file);

    tags = (ID3_tag*)(temp);  // копирование тегов

    if (strcmp(argv[2], "--show") == 0)
    {
        show(tags);
        return 0;
    }
    else if (strncmp(argv[2], "--get=", 6) == 0)
    {
        tag_to_change = strpbrk(argv[2], "=") + 1;
        get(tag_to_change, tags);
        return 0;
    }
    else if (strncmp(argv[2], "--set=", 6) == 0)
    {
        tag_to_change = strpbrk(argv[2], "=") + 1;
        if (strncmp(argv[3], "--value=", 8) == 0)
        {
            value_to_change = strpbrk(argv[3], "=") + 1;
            set(tag_to_change, value_to_change, tags);
        }
        else
        {
            printf("Unknown parameter of command '--value='\n");
        }
    }
    else
    {
        printf("Unknown command\n");
        return 0;
    }

    fclose(original_file);

    if ((changed_file = fopen(mp3_name, "wb+")) == NULL)
    {
        printf("An error occurred while opening file %s\n", mp3_name);
        return 0;
    }

    char *changed_tags = (char*) malloc(sizeof (char ) * ID3_max_size);
    memset(changed_tags, 0, ID3_max_size);

    changed_tags = (char*)tags;  // запись измененных тегов
    fseek(changed_file, file_offset(changed_file), SEEK_SET);

    fwrite(changed_tags, 1, ID3_max_size, changed_file);
    free(changed_tags);
    fclose(changed_file);

    return 0;
}