#ifndef OPER_WITH_STR
#define OPER_WITH_STR

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

struct StringData
{
    int index;
    char* begin_pointer;
    size_t length;
};

size_t count_entry_in_string(char* array, char symbol);
int strcmp_letters_from_beginning(const char* string_1, size_t len_1, const char* string_2, size_t len_2);
int strcmp_letters_from_end(const char* string_1, size_t len_1, const char* string_2, size_t len_2);
int get_string_data(StringData* data_strings, char* array, size_t len_array);
void print_struct(StringData* str, size_t len);

#endif