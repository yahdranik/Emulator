#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "spu_common.h"
#include "COMMON/StrOps.h"
#include "COMMON/FileOps.h"
#include "COMMON/sorts.h"

typedef char source_t[64];
typedef int source_elem_t;

const size_t SIZE_WITH_ARG    = sizeof(uint8_t) + sizeof(stack_elem_t);
const size_t SIZE_WITHOUT_ARG = sizeof(uint8_t);

#define SOURCE_ELEM_INIT 0xBEBE

#define FIRST_PASS if (pass_number == 1)
#define SECOND_PASS if (pass_number == 2)

// FILES ------------------------------------------------------------------------
#define SOURCE_FILE  "TESTS/gif_minecraft.asm"
#define LISTING_FILE "DEBUG/square_listing.list"
#define BIN_FILE     "TESTS/bytecode.bin"

// FILE HEADER -------------------------------------------------------------------
#define SPU_VERSION   7
#define DRM_MAGIC     0x004D5244 

// PROTOTYPES --------------------------------------------------------------------
spu_error_t translate_to_bytecode(FileData* source_info, spu_ctx* ctx, spu_label_t* labels, int pass_number);
spu_error_t comand_processing_to_bytecode(spu_ctx* ctx, char* arg, bytecode_t cmd, int* ip, spu_label_t* labels, size_t count_strings);
bytecode_t convert_one_comand(source_t cmd);
spu_error_t put_one_label(spu_label_t* labels, char* label, int ip, size_t* label_number);

int compare_for_label_names(const void* a, const void* b);
int find_adress(spu_label_t* labels, size_t size, source_t label);
int is_number(const char* arg);

void print_list(FILE* list, spu_ctx* ctx);

#endif