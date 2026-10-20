#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "COMMON/StrOps.h"
#include "COMMON/FileOps.h"
#include "processor.h"

typedef char source_t[64];
typedef int source_elem_t;

#define SOURCE_ELEM_INIT 0xBEBE

#define FIRST_PASS if (pass_number == 1)
#define SECOND_PASS if (pass_number == 2)

spu_error_t translate_to_bytecode(FileData* source_info, spu_ctx* ctx, spu_label_t* labels, int pass_number);
spu_error_t comand_processing_to_bytecode(spu_ctx* ctx, char* arg, bytecode_t cmd, int* ip, spu_label_t* labels, size_t count_strings);
bytecode_t convert_one_comand(source_t cmd);
spu_error_t translate_comands(FileData* source_info, spu_ctx* ctx, int pass_number);
spu_error_t put_one_label(spu_label_t* labels, char* label, int ip, size_t* label_number);
spu_error_t ctx_initialise(spu_ctx* ctx, size_t bytecode_size);
spu_error_t ctx_destroy(spu_ctx* ctx);

int find_adress(spu_label_t* labels, size_t size, source_t label);
int hash_labels(const char* variable);

void print_into_file(bytecode_t* bytecode, size_t size, FILE* code, FILE* code_bin);

#endif