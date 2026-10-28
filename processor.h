#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <cmath>
#include "spu_common.h"
#include "COMMON/FileOps.h"

#define SPU_DUMP(log, ctx)       spu_dump(log, ctx, __FILE__, __FUNCTION__, __LINE__)
#define PRINT_SPU_ERROR(code)    fprintf(log, "\nCode: %lld\n", code)

// FILES -------------------------------------------------------------------------------
#define LOG_FILE "DEBUG/blin_logs.log"
#define BIN_FILE "TESTS/bytecode.bin"

// FILE HEADER -------------------------------------------------------------------------
#define SPU_VERSION   7
#define DRM_MAGIC     0x004D5244 

// RAILIB DRAWING ----------------------------------------------------------------------
#include "C:\\raylib-6.0_win64_mingw-w64\\include\\raylib.h"
#include "C:\\raylib-6.0_win64_mingw-w64\\include\\rlgl.h"
#include "C:\\raylib-6.0_win64_mingw-w64\\include\\raymath.h"

bool is_window_open  = 0;

enum ScreenParametres
{
    INIT_CELL          = 32,
    VISIB_CELL         = 20,
    screenWidth        = INIT_CELL * VISIB_CELL,
    screenHeight       = INIT_CELL * VISIB_CELL,
    MAX_FPS            = 60
};

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
spu_error_t print(spu_cmd_args* args);
spu_error_t draw (spu_cmd_args* args);

void spu_dump(FILE* log, spu_ctx* ctx, const char* file_name, const char* func_name, int line);
void print_bytecode(FILE* log, const spu_ctx* ctx);
void print_regs(FILE* log, const stack_elem_t* array, int size);
void clean_buffer();

#endif