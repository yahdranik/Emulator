#include "cstdio"
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