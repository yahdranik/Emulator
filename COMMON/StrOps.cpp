#include "StrOps.h"

size_t count_entry_in_string(char* array, char symbol)
{
    size_t counter = 0;
    char* array_copy = array;
    char* entry = strchr(array_copy, symbol);

    while (entry != NULL)
    {
        counter += 1;
        array_copy = entry + 1;
        entry = strchr(array_copy, symbol);
    }

    return counter;
}

int get_string_data(StringData* data_strings, char* array, size_t len_array)
{
    if (data_strings == NULL || array == NULL || len_array == 0)
    {
        return -1;
    }
    
    size_t line_number = 0;
    char* begin_pointer = array;

    for (size_t i = 0; i < len_array; i++)
    {
        if (array[i] == '\n')
        {
            size_t length = (size_t) (array + i - begin_pointer);
            array[i] = '\0';

            data_strings[line_number] = (StringData) {(int) line_number, begin_pointer, length};
            begin_pointer = array + i + 1;
            line_number += 1;
        }
    }

    return 0;
}

int strcmp_letters_from_beginning(const char* string_1, size_t len_1, const char* string_2, size_t len_2)
{
    size_t pointer_1 = 0;
    size_t pointer_2 = 0;

    while (pointer_1 < len_1 && pointer_2 < len_2)
    {
        while (pointer_1 < len_1 && !isalpha(string_1[pointer_1]))
        {
            pointer_1 += 1;
        }

        while (pointer_2 < len_2 && !isalpha(string_2[pointer_2]))
        {
            pointer_2 += 1;
        }

        if (pointer_1 >= len_1 || pointer_2 >= len_2)
        {
            break;
        }

        int symbol_1 = tolower(string_1[pointer_1]);
        int symbol_2 = tolower(string_2[pointer_2]);

        if (symbol_1 != symbol_2)
        {
            return symbol_1 - symbol_2;
        }

        pointer_1 += 1;
        pointer_2 += 1;
    }
    return 0;
}

int strcmp_letters_from_end(const char* string_1, size_t len_1, const char* string_2, size_t len_2)
{
    size_t pointer_1 = len_1;
    size_t pointer_2 = len_2;

    while (pointer_1 > 0 && pointer_2 > 0)
    {
        while (pointer_1 > 0 && !isalpha(string_1[pointer_1 - 1]))
        {
            pointer_1 -= 1;
        }

        while (pointer_2 > 0 && !isalpha(string_2[pointer_2 - 1]))
        {
            pointer_2 -= 1;
        }

        if (pointer_1 == 0 || pointer_2 == 0)
        {
            break;
        }

        int symbol_1 = tolower(string_1[pointer_1 - 1]);
        int symbol_2 = tolower(string_2[pointer_2 - 1]);

        if (symbol_1 != symbol_2)
        {
            return symbol_1 - symbol_2;
        }

        pointer_1 -= 1;
        pointer_2 -= 1;
    }
    return 0;
}

// void print_struct(StringData* str, size_t len)
// {
//     for (size_t i = 0; i < len; i++)
//     {   
//         printf("%d\n", str[i].index);
//         printf("%s\n", str[i].begin_pointer);
//         printf("%lld\n", str[i].length);
//     }
// }