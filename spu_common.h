#ifndef SPU_COMMON_H
#define SPU_COMMON_H

#include <cstdint>
#include <cstdio>
#include <cstring>
#include "COMMON/stack_func.h"

// === CONSTS ===
#define BYTECODE_INIT 0xBE
#define BC_SP "%lg"

const int COUNT_REGS = 16;
const int COUNT_CMDS = 15;
const int MAX_LEN    = 64;
const int RAM_SIZE   = 1024;

const char* cmd_names[] = {"HLT", "PUSH", "ADD", "SUB", "DIV", "OUT", "IN", "JUMP", "CALL", "RET", "POP", "MUL", "SQR", "PRIN", "DRAW"};

// === MODES ===
#define NO_ARG         0x00
#define RAM_BYTE       0x04
#define CONST_BYTE     0x02
#define REG_BYTE       0x01

#define SIMPL_J_BYTE   0x00
#define J_EQ_BYTE      0x04
#define J_NEQ_BYTE     0x03
#define J_BELOW_BYTE   0x02
#define J_ABOVE_BYTE   0x01

// === DATA TYPES ===
typedef uint8_t bytecode_t;
typedef uint64_t error_t;

enum spu_error_t
{
    SPU_ERROR   = 1,
    SPU_SUCCESS = 0
};

enum spu_syntax_err_code_t
{
    SPU_REG_VALUE_ERROR = 1<<2,
    SPU_INCORRECT_ARG   = 1<<4,
    SPU_INCORRECT_CMD   = 1<<6,
    SPU_INCORRECT_LABEL = 1<<8
};

enum spu_runtime_err_code_t
{
    SPU_ALL_OK              = 0,
    SPU_NOT_MEMORY          = 1<<1,
    SPU_PUSH_ERROR          = 1<<3,
    SPU_POP_ERROR           = 1<<5,
    SPU_JUMP_ERROR          = 1<<7,
    SPU_CALL_ERROR          = 1<<9,
    SPU_RET_ERROR           = 1<<11,
    SPU_NOT_INIT            = 1<<13,
    SPU_DIV_BY_ZERO         = 1<<15,
    SPU_SQRT_FROM_NEG       = 1<<17,
    SPU_FILE_READ_INCORRECT = 1<<19
};

enum spu_cmd_code_t
{
    HLT   = 0x00,
    PUSH  = 0x01,
    ADD   = 0x02,
    SUB   = 0x03,
    DIV   = 0x04,
    OUT   = 0x05,
    IN    = 0x06,
    JUMP  = 0x07,
    CALL  = 0x08,
    RET   = 0x09,
    POP   = 0x0A,
    MUL   = 0x0B,
    SQR   = 0x0C,
    PRINT = 0x0D,
    DRAW  = 0x0E,

// === PSEUDOCOMANDS ===
    JNE   = 0x0F,
    JE    = 0x10,
    JA    = 0x11,
    JB    = 0x12,
    JAE   = 0x13,
    JBE   = 0x14
};

struct spu_header_t
{
    uint32_t magic;         
    uint32_t version;       
    uint32_t bytecode_size; 
};

struct spu_ctx
{
    bytecode_t*    bytecode;
    stack_elem_t   regs[COUNT_REGS];
    stack_t*       func_rets;
    stack_elem_t*  ram;

    int            current_ip;
    int            bytecode_size;
    size_t         label_count; 
    error_t        error;
};

struct spu_cmd_args
{
    spu_ctx*        ctx;
    stack_t*        stack;
    stack_elem_t    raw_arg;        
    uint8_t         mode;           
    int*            ip;
};

struct spu_label_t
{
    char name[MAX_LEN];
    int adress;
};

spu_error_t ctx_initialise(spu_ctx* ctx, int bytecode_size);
spu_error_t ctx_destroy(spu_ctx* ctx);

#endif