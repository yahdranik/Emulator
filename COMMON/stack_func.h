#ifndef STACK_FUNC_H
#define STACK_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cstdio>
#include <stdint.h>

// #define CANARY_DEBUG
// #define SHADOW_DEBUG
// #define HASHES_DEBUG 

#define PRINT_STK_ERROR(code) fprintf(log, "\nCode: %d\n", code)
#define LOG(level, message) fprintf(log, "%s:  %s\n", level, message)
#define CHECK_STK_ERROR(error, flag) (((error) & (flag)) == (flag))

#if defined(CANARY_DEBUG) || defined(SHADOW_DEBUG) || defined(HASHES_DEBUG)
#define STACK_DUMP(log, variable) stack_dump(log, variable, __FILE__, __FUNCTION__, __LINE__)
#else
#define STACK_DUMP(log, variable) ((void)0)
#endif

#define STACK_ELEM_INIT 0xBEBE
#define MY_SP "%0.6lg"

#define FNV_BASIS 2166136261u
#define FNV_PRIME 16777619u

#ifdef CANARY_DEBUG
#define CANARY_1 0xBEDA1
#define CANARY_2 0xBEDA2
#endif 

typedef double stack_elem_t;
typedef uint64_t error_t;
const int FACTOR = 2;

enum stack_err_codes_t
{
    STK_ALL_OKAY            = 0,
    STK_NOT_REALLOCATE      = 1<<0,
    STK_NULL_CAPACITY       = 1<<1,
    STK_NULL_ADRESS         = 1<<2,
    STK_INCORRECT_SIZE      = 1<<3,
    STK_OUT_OF_BOUNDS       = 1<<4,
    STK_BAD_STACK           = 1<<5,
    STK_STRUCT_DAMAGE       = 1<<6
};

enum stack_err_t
{
    STK_SUCCESS = 0,
    STK_ERROR   = 1
};

struct stack_t 
{
    #ifdef CANARY_DEBUG
    stack_elem_t canary_begin;
    #endif 

    stack_elem_t* data;
    size_t size;
    size_t capacity;
    error_t error;

    #ifdef CANARY_DEBUG
    stack_elem_t canary_end;
    #endif 

    #ifdef SHADOW_DEBUG
    stack_elem_t* data_copy;
    #endif 

    #ifdef HASHES_DEBUG
    uint32_t hash;
    #endif
};

stack_err_t stack_realloc_up(stack_t* variable);
stack_err_t stack_initialise(stack_t* variable, size_t init_capacity);
stack_err_t stack_verify(stack_t* variable);
stack_err_t stack_push(stack_t* variable, stack_elem_t elem);
stack_err_t stack_pop(stack_t* variable, stack_elem_t* elem);
stack_err_t stack_realloc_down(stack_t* variable);
int stack_dump(FILE* log, stack_t* variable, const char* file_name, const char* func_name, int line);
stack_err_t stack_destroy(stack_t* variable);
int is_equal(stack_elem_t elem_1, stack_elem_t elem_2);
void print_arrays(FILE* log, stack_elem_t* array, stack_t* variable);

#ifdef HASHES_DEBUG
uint32_t hash_bytes(const void* variable);
#endif

#endif