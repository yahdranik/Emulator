#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#define MY_END_CUSTOM          "\033[0m"
#define MY_BLUE_AND_CURSIVE    "\033[3;34m"
#define MY_GREEN_AND_CURSIVE   "\033[3;32m"
#define MY_RED_AND_BOLD        "\033[1;31m"

struct Houses
{
    int house_number;
    int count_of_flours;
};

void bubble_sort(int* array, int len_array);
void print_array(int* array, size_t len_array);
void swap(void* a, void* b, size_t size_element);
void shaker_sort(int* array, int len_array);
void quick_sort(void* array, size_t len_array, size_t size_element, int (*compare)(const void* a, const void* b));
int compare_for_const_char(const void* a, const void* b);
int compare_for_int(const void* a, const void* b);
int compare_for_struct_Houses(const void* a, const void* b);
int compare_for_double(const void* a, const void* b);
// int is_equal_array(void* a, void* b, size_t size_element, size_t count);
int compare_for_char(const void* a, const void* b);