#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <cstdlib>
#include <stdio.h>
#include <cmath>
#include "COMMON/FileOps.h"
#include "COMMON/StrOps.h"
#include "COMMON/stack_func.h"

#define BYTECODE_INIT 0xBEBE
#define BC_SP "%lg"
#define SPU_DUMP(log, ctx)       spu_dump(log, ctx, __FILE__, __FUNCTION__, __LINE__)
#define PRINT_SPU_ERROR(code)    fprintf(log, "\nCode: %d\n", code)
#define LOG(level, message)      fprintf(log, "%s:  %s\n", level, message)

#define CMD_NAME(cmd) (is_equal(cmd, HLT  ) ? "HLT"   :        \
                       is_equal(cmd, PUSH ) ? "PUSH"  :        \
                       is_equal(cmd, PUSHr) ? "PUSHr" :        \
                       is_equal(cmd, POP  ) ? "POP"   :        \
                       is_equal(cmd, POPr ) ? "POPr"  :        \
                       is_equal(cmd, OUT  ) ? "OUT"   :        \
                       is_equal(cmd, IN   ) ? "IN"    :        \
                       is_equal(cmd, DIV  ) ? "DIV"   :        \
                       is_equal(cmd, SUB  ) ? "SUB"   :        \
                       is_equal(cmd, ADD  ) ? "ADD"   :        \
                       is_equal(cmd, MULT ) ? "MULT"  :        \
                       is_equal(cmd, JUMP ) ? "JUMP"  :        \
                       is_equal(cmd, JE   ) ? "JE"    :        \
                       is_equal(cmd, JNE  ) ? "JNE"   :        \
                       is_equal(cmd, CALL ) ? "CALL"  :        \
                       is_equal(cmd, RET  ) ? "RET"   :        \
                       is_equal(cmd, SQR  ) ? "SQR"   : "UNKNOWN COMAND")

typedef double bytecode_t;
typedef uint64_t error_t;

const int COUNT_REGS = 16;
const int COUNT_CMDS = 17;
const int MAX_LEN    = 64;

enum spu_error_t
{
    SPU_ERROR   = 1,
    SPU_SUCCESS = 0
};

enum spu_syntax_err_code_t
{
    SPU_REG_VALUE_ERROR = 1<<8,
    SPU_INCORRECT_ARG   = 1<<9,
    SPU_INCORRECT_CMD   = 1<<11
};

enum spu_runtime_err_code_t
{
    SPU_ALL_OK          = 0,
    SPU_NOT_MEMORY      = 1<<1,
    SPU_PUSH_ERROR      = 1<<2,
    SPU_POP_ERROR       = 1<<3,
    SPU_JUMP_ERROR      = 1<<4,
    SPU_CALL_ERROR      = 1<<5,
    SPU_RET_ERROR       = 1<<6,
    SPU_NOT_INIT        = 1<<7,
    SPU_DIV_BY_ZERO     = 1<<10,
    SPU_SQRT_FROM_NEG   = 1<<12
};

enum spu_cmd_code_t
{
    HLT     = 0,
    PUSH    = 1,
    ADD     = 2,
    SUB     = 3,
    DIV     = 4,
    OUT     = 5,
    IN      = 6,
    JUMP    = 7,
    JE      = 8,
    JNE     = 9,
    PUSHr   = 10,
    POPr    = 11,
    CALL    = 12,
    RET     = 13,
    POP     = 14,
    MULT    = 15,
    SQR    = 16
};

struct spu_ctx
{
    bytecode_t*    bytecode;
    stack_elem_t   regs[COUNT_REGS];
    stack_t*       func_rets;

    int            current_ip;
    int            bytecode_size;
    error_t        error;
};

struct spu_cmd_args
{
    spu_ctx*        ctx;
    stack_t*        stack;
    stack_elem_t    arg;
    int*            ip;
};

struct spu_label_t
{
    char name[MAX_LEN];
    bytecode_t adress;
};

spu_error_t ctx_initialise(spu_ctx* ctx, size_t bytecode_size);
spu_error_t get_bytecode(FileData* bytecode_info, spu_ctx* ctx);
spu_error_t processing_bytecode(spu_ctx* ctx, int size, stack_t* stack);

spu_error_t push(spu_cmd_args* args);
spu_error_t pushr(spu_cmd_args* args);
spu_error_t popr(spu_cmd_args* args);
spu_error_t add(spu_cmd_args* args);
spu_error_t sub(spu_cmd_args* args);
spu_error_t div(spu_cmd_args* args);
spu_error_t out(spu_cmd_args* args);
spu_error_t in(spu_cmd_args* args);
spu_error_t je(spu_cmd_args* args);
spu_error_t jne(spu_cmd_args* args);
spu_error_t hlt(spu_cmd_args* args);
spu_error_t jump(spu_cmd_args* args);
spu_error_t call(spu_cmd_args* args);
spu_error_t ret(spu_cmd_args* args);
spu_error_t pop(spu_cmd_args* args);
spu_error_t mult(spu_cmd_args* args);
spu_error_t sqr(spu_cmd_args* args);

void spu_dump(FILE* log, spu_ctx* ctx, const char* file_name, const char* func_name, int line);
void print_bytecode(FILE* log, const spu_ctx* ctx);
void print_regs(FILE* log, const stack_elem_t* array, size_t size);
void clean_buffer();
int is_number(const char* arg);

#endif