#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <cmath>
#include "spu_common.h"
#include "COMMON/FileOps.h"

#define SPU_DUMP(log, ctx)       spu_dump(log, ctx, __FILE__, __FUNCTION__, __LINE__)
#define PRINT_SPU_ERROR(code)    fprintf(log, "\nCode: %d\n", code)

// FILE HEADER -------------------------------------------------------------------------
#define SPU_VERSION   1
#define DRM_MAGIC     0x004D5244 

// PROTOTYPES --------------------------------------------------------------------------
spu_error_t get_bytecode(const char* file_name, spu_ctx* ctx);
spu_error_t processing_bytecode(spu_ctx* ctx, stack_t* stack);

spu_error_t push(spu_cmd_args* args);
spu_error_t add(spu_cmd_args* args);
spu_error_t sub(spu_cmd_args* args);
spu_error_t div(spu_cmd_args* args);
spu_error_t out(spu_cmd_args* args);
spu_error_t in(spu_cmd_args* args);
spu_error_t hlt(spu_cmd_args* args);
spu_error_t jump(spu_cmd_args* args);
spu_error_t call(spu_cmd_args* args);
spu_error_t ret(spu_cmd_args* args);
spu_error_t pop(spu_cmd_args* args);
spu_error_t mult(spu_cmd_args* args);
spu_error_t sqr(spu_cmd_args* args);
spu_error_t draw(spu_cmd_args* args);

void spu_dump(FILE* log, spu_ctx* ctx, const char* file_name, const char* func_name, int line);
void print_bytecode(FILE* log, const spu_ctx* ctx);
void print_regs(FILE* log, const stack_elem_t* array, size_t size);
void clean_buffer();

#endif