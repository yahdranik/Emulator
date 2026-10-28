#include "sorts.h"

void bubble_sort(int* array, int len_array)
{
    for (int i = 0; i < len_array - 1; i++)
    {
        for (int j = 0; j < len_array - 1 - i; j++)
        {
            if (array[j] > array[j + 1])
            {
                swap(&array[j], &array[j + 1], sizeof(array[0]));
            }
        }
    }
}

void shaker_sort(int* array, int len_array)
{
    int complite = 1;
    int right = len_array;
    int left = 0;
    while (complite != 0)
    {
        complite = 0;
        
        for (int j = left; j < right - 1; j++)
        {
            if (array[j] > array[j + 1])
            {
                swap(&array[j], &array[j + 1], sizeof(array[0]));
                complite = 1;
            }
        }

        for (int i = right - 1; i > left; i--)
        {
            if (array[i] < array[i - 1])
            {
                swap(&array[i - 1], &array[i], sizeof(array[0]));
                complite = 1;
            }            
        }
    }
}

void quick_sort(void* array, size_t len_array, size_t size_element, int (*compare)(const void* a, const void* b))
{
    uint8_t* ptr = (uint8_t *) array;

    if (len_array <= 1)
    {
        return;
    }

    uint8_t* pivot = ptr;
    uint8_t* left = ptr + size_element;
    uint8_t* right = ptr + len_array * size_element - size_element;

    while ( right >= left )
    {
        while (right >= left && compare(left, pivot) < 0)
        {
            left += size_element;
        }
        
        while (right >= left && compare(right, pivot) > 0)
        {
            right -= size_element;
        }

        if (left <= right)
        {
            swap(left, right, size_element);
            left += size_element;
            right -= size_element;
        }
    }

    swap(pivot, right, size_element);
    quick_sort(ptr, (right - ptr) / size_element, size_element, compare);
    quick_sort(right + size_element, len_array - (right - ptr) / size_element - 1, size_element, compare);
}

void print_array(int* array, size_t len_array)
{   
    for (size_t i = 0; i < len_array; i++)
    {
        if (array[i] != 0)
        {
            printf("%d ", array[i]);
        }
    }
    printf("\n");
}

void swap(void* a, void* b, size_t size_element)
{
    uint8_t* elem_1 = (uint8_t*)a;
    uint8_t* elem_2 = (uint8_t*)b;
    
    for (size_t i = 0; i < size_element; i++) 
    {
        uint8_t temp = elem_1[i];
        elem_1[i] = elem_2[i];
        elem_2[i] = temp;
    }
}
int compare_for_int(const void* a, const void* b)
{
    const int* elem_1 = (const int*) a;
    const int* elem_2 = (const int*) b;
    return (*elem_1 > *elem_2) - (*elem_1 < *elem_2) ;
}

int compare_for_const_char(const void* a, const void* b)
{
    const char* elem_1 = *(const char**) a;
    const char* elem_2 = *(const char**) b;
    return strcmp(elem_1, elem_2);
}

int compare_for_struct_Houses(const void* a, const void* b)
{
    const struct Houses* elem_1 = (const struct Houses*) a;
    const struct Houses* elem_2 = (const struct Houses*) b;
    return (elem_1->house_number > elem_2->house_number) - (elem_1->house_number < elem_2->house_number);
}

int compare_for_char(const void* a, const void* b)
{
    const char* elem_1 = (const char*) a;
    const char* elem_2 = (const char*) b;
    return strcmp(elem_1, elem_2);
}

int compare_for_double(const void* a, const void* b)
{
    const double* elem_1 = (const double*) a;
    const double* elem_2 = (const double*) b;
    return (*elem_1 > *elem_2) - (*elem_1 < *elem_2);
}

