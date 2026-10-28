#include "StrOps.h"
#include "FileOps.h"
ssize_t get_file_size(const char* NAME)
{
    struct stat file_info = {};
    stat(NAME, &file_info);
    ssize_t size_of_file = file_info.st_size;

    if (size_of_file == -1 || size_of_file == 0)
    {
        perror("Size unvalible\n");
        return UNVALIBLE_FILE_SIZE;
    }

    return size_of_file;
}

struct FileData read_file_and_get_data(const char* FILE_NAME)
{
    size_t size_of_file = get_file_size(FILE_NAME);
    if (size_of_file == UNVALIBLE_FILE_SIZE) { return {}; }

    FILE* text = fopen(FILE_NAME, "r");
    if (text == NULL) { perror("File not open\n"); return {}; }

    char* original_text = (char*) calloc(size_of_file + 1, sizeof(char));
    if (original_text == NULL) { fclose(text); return {}; }

    ssize_t bytes_read = fread(original_text, sizeof(char), size_of_file, text);
    if (bytes_read == -1) { fclose(text); FREE_MASS(original_text); perror("File do not read\n"); return {}; }

    fclose(text);

    size_t count_of_strings = count_entry_in_string(original_text, '\n');

    if (bytes_read > 0 && original_text[bytes_read - 1] != '\n')
    {
        original_text[bytes_read - 1] = '\n';
        count_of_strings += 1;
    }

    struct FileData file_info = {(size_t) bytes_read, count_of_strings, original_text};

    return file_info;
}

int print_strings_into_file(FILE* filestream, StringData* array, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        if (array[i].begin_pointer != NULL)
        {
            fprintf(filestream, "%s\n", array[i].begin_pointer);
        }
    }

    return 0;
}

int print_into_file(FILE* filestream, const FileData file_info, StringData* array, int mode)
{
    switch(mode)
    {
    case SORT_FOR_BEGIN:

        fprintf(filestream, "\n-----------------------------------------------------------------------------------\n\n");
        fprintf(filestream, "SORT FOR BEGIN\n");
        fprintf(filestream, "\n-----------------------------------------------------------------------------------\n\n");
        print_strings_into_file(filestream, array, file_info.count_of_strings);
        break;

    case SORT_FOR_END:

        fprintf(filestream, "\n-----------------------------------------------------------------------------------\n\n");
        fprintf(filestream, "SORT FOR END\n");
        fprintf(filestream, "\n-----------------------------------------------------------------------------------\n\n");
        print_strings_into_file(filestream, array, file_info.count_of_strings);
        break;

    case ORIGINAL:

        fprintf(filestream, "\n-----------------------------------------------------------------------------------\n\n");
        fprintf(filestream, "THE ORIGINAL\n");
        fprintf(filestream, "\n-----------------------------------------------------------------------------------\n\n");
        print_strings_into_file(filestream, array, file_info.count_of_strings);
        break;
    
    default:
        printf("Text don`t print\n");
        return INCORRECT_MODE_FOR_PRINT;
    }
    
    return 0;
}
