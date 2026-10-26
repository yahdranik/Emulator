#include "processor.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>

// TODO:  дамп
//TODO: ошибки по типу, синтаксические, рантайм и тд
// хранение в бинарном виде, листинг
// сделать еще один хэдер для общих функций и констант транслятора и процессора

// спросить: как писать скрипты на питоне и как ваще писать асм для графики
int main()
{
    FILE* log =  fopen("TOOLS/blin_logs.log", "w");
    stack_t stack = {};
    stack_err_t res_stack_init = stack_initialise(&stack, 16);
    if (res_stack_init == STK_ERROR) {printf("Stack dont init\n"); return SPU_ERROR;}

    ssize_t size = get_file_size("TESTS/bytecode.bin");
    if (size < 0) 
    {
        printf("file size is incorrect\n");
        stack_destroy(&stack);
        fclose(log);
        return SPU_ERROR;
    }

    spu_ctx ctx = {};
    spu_error_t res_ctx_init = ctx_initialise(&ctx, (int) size);
    if (res_ctx_init == SPU_ERROR) 
    {
        ctx_destroy(&ctx);
        stack_destroy(&stack);
        fclose(log);
    }

    get_bytecode("TESTS/bytecode.bin", &ctx);

    SPU_DUMP(log, &ctx);

    processing_bytecode(&ctx, &stack);
    ctx_destroy(&ctx);
    stack_destroy(&stack);

    fclose(log);
}

spu_error_t ctx_initialise(spu_ctx* ctx, int bytecode_size)
{
    if (ctx == NULL) {return SPU_ERROR;}

    ctx -> bytecode = (bytecode_t*) calloc(bytecode_size, sizeof(bytecode_t));
    if (ctx->bytecode == NULL) {ctx -> error |= SPU_NOT_MEMORY; return SPU_ERROR;}

    for (size_t i = 0; i < COUNT_REGS; i++) 
    {
        ctx->regs[i] = BYTECODE_INIT;
    }

    ctx->func_rets = (stack_t*) calloc(1, sizeof(stack_t));
    if (ctx->func_rets == NULL) { ctx->error |= SPU_NOT_MEMORY; return SPU_ERROR; }

    stack_err_t res_init = stack_initialise(ctx -> func_rets, bytecode_size);
    if (res_init == STK_ERROR) {ctx->error |= SPU_NOT_INIT; return SPU_ERROR;}
    ctx->error = SPU_ALL_OK;

    ctx->bytecode_size = bytecode_size;

    return SPU_SUCCESS;
}

spu_error_t ctx_destroy(spu_ctx* ctx)
{
    if (ctx == NULL) return SPU_ERROR;

    free(ctx->bytecode);
    ctx->bytecode = NULL;

    if (ctx->func_rets != NULL)
    {
        stack_destroy(ctx->func_rets);
        free(ctx->func_rets);
        ctx->func_rets = NULL;
    }

    ctx->error = SPU_ALL_OK;

    return SPU_SUCCESS;
}

spu_error_t get_bytecode(const char* file_name, spu_ctx* ctx)
{
    FILE* bin_code = fopen(file_name, "rb");
    if (bin_code == NULL) 
    {
        printf("Error: Cannot open file %s\n", file_name);
        ctx->error |= SPU_FILE_READ_INCORRECT;
        return SPU_ERROR;
    }

    spu_header_t header = {};
    if (fread(&header, sizeof(spu_header_t), 1, bin_code) != 1) 
    {
        printf("Error: File has not header\n");
        fclose(bin_code);
        ctx->error |= SPU_FILE_READ_INCORRECT;
        return SPU_ERROR;
    }

    if (header.magic != DRM_MAGIC) 
    {
        printf("Error: Invalid file format\n");
        fclose(bin_code);
        ctx->error |= SPU_FILE_READ_INCORRECT;
        return SPU_ERROR;
    }

    if (header.version != SPU_VERSION) 
    {
        printf("Error: Version do not match. File: %d, Processor: %d\n", header.version, SPU_VERSION);
        fclose(bin_code);
        ctx->error |= SPU_FILE_READ_INCORRECT;
        return SPU_ERROR;
    }

    if (ctx->bytecode_size != (int)header.bytecode_size)
    {
        free(ctx->bytecode);
        ctx->bytecode = (bytecode_t*)calloc(header.bytecode_size, sizeof(bytecode_t));
        if (ctx->bytecode == NULL) 
        {
            fclose(bin_code);
            ctx->error |= SPU_NOT_MEMORY;
            return SPU_ERROR;
        }
        ctx->bytecode_size = header.bytecode_size;
    }

    int elem_reads = fread(ctx->bytecode, sizeof(uint8_t), ctx->bytecode_size, bin_code);

    fclose(bin_code);
    return SPU_SUCCESS;
}

spu_error_t processing_bytecode(spu_ctx* ctx, stack_t* stack)
{
    int size = ctx->bytecode_size;
    int ip = 0;
    spu_error_t (*comands[]) (spu_cmd_args*) = {hlt, push, add, sub, div, out, in, jump, call, ret, pop, mult, sqr, draw};

    ctx->current_ip = ip;

    while(ip < size)
    {
        int cmd = (int) ctx->bytecode[ip]; 

        uint8_t opcode = (uint8_t) cmd >> 3;
        uint8_t mode   = (uint8_t) cmd & 0x07;

        if (opcode >= COUNT_CMDS) { ctx->error |= SPU_INCORRECT_CMD; return SPU_ERROR; }

        spu_cmd_args cmd_args = {ctx, stack, 0.0, mode, &ip};

        if (opcode == PUSH || opcode == POP || opcode == JUMP || opcode == CALL)
        {
            memcpy(&cmd_args.raw_arg, &ctx->bytecode[ip + 1], sizeof(stack_elem_t));
        }

        spu_error_t res_ex_cmd = comands[opcode](&cmd_args);
        if (res_ex_cmd == SPU_ERROR) return SPU_ERROR;
    }

    return SPU_SUCCESS;
}

spu_error_t push(spu_cmd_args* args)
{
    stack_elem_t arg_value = 0;

    if ((args->mode & RAM_BYTE) == RAM_BYTE)
    {
        int adress = 0;
        
        if ((args->mode & REG_BYTE) == REG_BYTE)
        {
            int reg = (int) args->raw_arg;
            if (reg < 0 || reg >= COUNT_REGS) { args->ctx->error |= SPU_INCORRECT_ARG; return SPU_ERROR; }
            adress = (int) args->ctx->regs[reg];
        }

        else 
        {
            adress = (int) args->raw_arg;
        }

        if (adress >= RAM_SIZE || adress < 0) { args->ctx->error |= SPU_INCORRECT_ARG; return SPU_ERROR; }
        
        arg_value = (stack_elem_t) args->ctx->ram[adress];
    }
    
    else if ((args->mode & CONST_BYTE) == CONST_BYTE)
    {
        arg_value = args->raw_arg;
    }
    
    else if ((args->mode & REG_BYTE) == REG_BYTE)
    {
        int reg = (int) args->raw_arg;
        
        if (reg < 0 || reg >= COUNT_REGS) { args->ctx->error |= SPU_INCORRECT_ARG; return SPU_ERROR; }
        arg_value = args->ctx->regs[reg];
    }
    
    else 
    {
        args->ctx->error |= SPU_INCORRECT_ARG;
        return SPU_ERROR;
    }

    stack_err_t res_push = stack_push(args->stack, arg_value);
    if (res_push != STK_SUCCESS) args->ctx->error |= SPU_PUSH_ERROR;

    *(args->ip) += 1 + sizeof(stack_elem_t);

    return SPU_SUCCESS;
}

spu_error_t pop(spu_cmd_args* args)
{
    stack_elem_t arg_value = 0;

    if (stack_pop(args->stack, &arg_value) != STK_SUCCESS) 
    {
        args->ctx->error |= SPU_POP_ERROR;
        return SPU_ERROR;
    }

    if ((args->mode & RAM_BYTE) == RAM_BYTE)
    {
        int adress = 0;

        if ((args->mode & REG_BYTE) == REG_BYTE) 
        {
            int reg = (int) args->raw_arg;
            if (reg < 0 || reg >= COUNT_REGS) { args->ctx->error |= SPU_INCORRECT_ARG; return SPU_ERROR; }
            adress = (int) args->ctx->regs[reg];
        } 

        else
        {
            adress = (int) args->raw_arg;
        }
        
        if (adress >= RAM_SIZE || adress < 0) { args->ctx->error |= SPU_INCORRECT_ARG; return SPU_ERROR; }
        
        args->ctx->ram[adress] = arg_value;
    }

    else if ((args->mode & REG_BYTE) == REG_BYTE)
    {
        int reg = (int) args->raw_arg;
        if (reg < 0 || reg >= COUNT_REGS) { args->ctx->error |= SPU_INCORRECT_ARG; return SPU_ERROR; }

        args->ctx->regs[reg] = arg_value;
    }

    else
    {
        args->ctx->error |= SPU_INCORRECT_ARG;
        return SPU_ERROR;
    }

    *(args->ip) += 1 + sizeof(stack_elem_t);

    return SPU_SUCCESS;
}

spu_error_t add(spu_cmd_args* args)
{
    stack_elem_t elem_1 = BYTECODE_INIT;
    stack_elem_t elem_2 = BYTECODE_INIT;

    stack_err_t res_pop_1 = stack_pop(args -> stack, &elem_1);
    if (res_pop_1 != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;

    stack_err_t res_pop_2 = stack_pop(args -> stack, &elem_2);
    if (res_pop_2 != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;

    stack_err_t res_push = stack_push(args -> stack, elem_1 + elem_2);
    if (res_push != STK_SUCCESS) args->ctx->error |= SPU_PUSH_ERROR;

    *(args -> ip) += 1;

    return SPU_SUCCESS;
}

spu_error_t sub(spu_cmd_args* args)
{
    stack_elem_t elem_1 = BYTECODE_INIT;
    stack_elem_t elem_2 = BYTECODE_INIT;

    stack_err_t res_pop_1 = stack_pop(args -> stack, &elem_1);
    if (res_pop_1 != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;

    stack_err_t res_pop_2 = stack_pop(args -> stack, &elem_2);
    if (res_pop_2 != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;

    stack_err_t res_push = stack_push(args -> stack, elem_2 - elem_1);
    if (res_push != STK_SUCCESS) args->ctx->error |= SPU_PUSH_ERROR;

    *(args -> ip) += 1;

    return SPU_SUCCESS;
}

spu_error_t div(spu_cmd_args* args)
{
    stack_elem_t elem_1 = BYTECODE_INIT;
    stack_elem_t elem_2 = BYTECODE_INIT;

    stack_err_t res_pop_1 = stack_pop(args -> stack, &elem_1);
    if (res_pop_1 != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;

    stack_err_t res_pop_2 = stack_pop(args -> stack, &elem_2);
    if (res_pop_2 != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;

    if (is_equal(elem_1, 0)) {args->ctx->error |= SPU_DIV_BY_ZERO;}
    stack_err_t res_push = stack_push(args -> stack, elem_2 / elem_1);
    if (res_push != STK_SUCCESS) args->ctx->error |= SPU_PUSH_ERROR;

    *(args -> ip) += 1;

    return SPU_SUCCESS;
}

spu_error_t mult(spu_cmd_args* args)
{
    stack_elem_t elem_1 = BYTECODE_INIT;
    stack_elem_t elem_2 = BYTECODE_INIT;

    stack_err_t res_pop_1 = stack_pop(args -> stack, &elem_1);
    if (res_pop_1 != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;

    stack_err_t res_pop_2 = stack_pop(args -> stack, &elem_2);
    if (res_pop_2 != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;


    stack_err_t res_push = stack_push(args -> stack, elem_2 * elem_1);
    if (res_push != STK_SUCCESS) args->ctx->error |= SPU_PUSH_ERROR;

    *(args -> ip) += 1;

    return SPU_SUCCESS;
}

spu_error_t sqr(spu_cmd_args* args)
{
    stack_elem_t elem = BYTECODE_INIT;

    stack_err_t res_pop = stack_pop(args -> stack, &elem);
    if (res_pop != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;

    if (elem < 0) args->ctx->error |= SPU_SQRT_FROM_NEG;

    stack_err_t res_push = stack_push(args -> stack, sqrt(elem));
    if (res_push != STK_SUCCESS) args->ctx->error |= SPU_PUSH_ERROR;

    *(args -> ip) += 1;

    return SPU_SUCCESS;
}

spu_error_t out(spu_cmd_args* args)
{
    stack_elem_t value = BYTECODE_INIT;

    stack_err_t res_pop = stack_pop(args -> stack, &value);
    if (res_pop != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;

    printf( BC_SP "\n", value);

    *(args -> ip) += 1;

    return SPU_SUCCESS;
}

spu_error_t in(spu_cmd_args* args)
{
    stack_elem_t user_value = BYTECODE_INIT;
    scanf(BC_SP , &user_value);

    while (is_equal(user_value, BYTECODE_INIT))
    {
        printf("wrong value data type. Enter again!");
        clean_buffer();
        scanf(BC_SP, &user_value);
    }

    stack_err_t res_push = stack_push(args -> stack, user_value);
    if (res_push != STK_SUCCESS) args->ctx->error |= SPU_PUSH_ERROR;

    *(args -> ip) += 1;

    return SPU_SUCCESS;
}

void clean_buffer() 
{
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF);
}

spu_error_t jump(spu_cmd_args* args)
{
    int adress = (int) args->raw_arg;
    if (adress < 0 || adress >= args->ctx->bytecode_size)
    {
        args->ctx->error |= SPU_INCORRECT_ARG;
        return SPU_ERROR;
    }

    if (args->mode == SIMPL_J_BYTE)
    {
        *(args->ip) = adress;
        return SPU_SUCCESS;
    }

    stack_elem_t a = 0, b = 0;
    if (stack_pop(args->stack, &a) != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;
    if (stack_pop(args->stack, &b) != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;

    bool flag = false;
    if      (args->mode == J_EQ_BYTE)    flag =  is_equal(b, a);
    else if (args->mode == J_NEQ_BYTE)   flag = !is_equal(b, a);
    else if (args->mode == J_BELOW_BYTE) flag =  (b < a);
    else if (args->mode == J_ABOVE_BYTE) flag =  (b > a);

    if (flag) *(args->ip) = adress;
    else      *(args->ip) += 1 + sizeof(stack_elem_t);

    return SPU_SUCCESS;
}

spu_error_t call(spu_cmd_args* args)
{
    stack_err_t res_push = stack_push(args -> ctx -> func_rets, *args -> ip + 9);
    if (res_push != STK_SUCCESS) args->ctx->error |= SPU_PUSH_ERROR;

    spu_error_t res_jump = jump(args);
    if (res_jump == SPU_ERROR) return SPU_ERROR;

    return SPU_SUCCESS;
}

spu_error_t ret(spu_cmd_args* args)
{
    stack_elem_t ret_adress = STACK_ELEM_INIT;

    stack_err_t res_pop = stack_pop(args -> ctx -> func_rets, &ret_adress);
    if (res_pop != STK_SUCCESS) args -> ctx -> error |= SPU_POP_ERROR; 

    spu_cmd_args args_ret = {args -> ctx, args -> stack, ret_adress, SIMPL_J_BYTE, args -> ip};

    spu_error_t res_jump = jump(&args_ret);
    if (res_jump == SPU_ERROR) return SPU_ERROR;

    return SPU_SUCCESS;
}

spu_error_t hlt(spu_cmd_args* args)
{
    printf("HLT summoned in %d comand\n", *(args -> ip));

    ctx_destroy(args->ctx);
    stack_destroy(args->stack);

    exit(0);
    return SPU_SUCCESS;
}

spu_error_t draw(spu_cmd_args* args)
{
    for (int i = 0; i < RAM_SIZE; i++)
    {
        int c = (int) args->ctx->ram[i];
        printf("%c", c);
    }

    *(args->ip) += 1;
    return SPU_SUCCESS;
}

void spu_dump(FILE* log, spu_ctx* ctx, const char* file_name, const char* func_name, int line)
{
    int safe_ip = ctx->current_ip;
    if (safe_ip < 0 || safe_ip >= ctx->bytecode_size) safe_ip = 0;

    bytecode_t cur_cmd = ctx->bytecode[safe_ip];

    PRINT_SPU_ERROR(ctx->error);
    fprintf(log, "Dump was summoned in\n"
                 "      file: <%s>\n"       
                 "      function: <%s>\n"      
                 "      line: <%d>\n", 
            file_name, func_name, line);

    fprintf(log, "\nCurrent parametres:\n");

    fprintf(log, "      struct: ctx[%p]\n"
                 "      bytecode: variable -> stack[%p]\n"
                 "      regs: ctx -> regs[%p]\n"
                 "      func returnes: ctx -> func_rets[%p]\n"
                 "      \n"
                 "      current ip: %d"
                 "      current comand: %02X (%s)"
                 "      bytecode size: %d",
                 ctx, ctx->bytecode, ctx->regs, ctx->func_rets, safe_ip, 
                 cur_cmd, CMD_NAME(cur_cmd >> 3), ctx->bytecode_size);

    fflush(log);
    fprintf(log, "\n\nBytecode:\n");
    print_bytecode(log, ctx);

    fprintf(log, "\n\nRegisters:\n");
    print_regs(log, ctx->regs, COUNT_REGS);
    
    fflush(log);
}

void print_bytecode(FILE* log, const spu_ctx* ctx)
{
    int start = (ctx->current_ip - 30 > 0) ? ctx->current_ip - 30 : 0;
    int end   = (ctx->current_ip + 30 < ctx->bytecode_size) ? ctx->current_ip + 30 : ctx->bytecode_size;

    for (int i = start; i < end; i++)
        fprintf(log, "%04d  ", i);
    fprintf(log, "\n");

    for (int i = start; i < end; i++)
    {
        if (i == ctx->current_ip)
            fprintf(log, "[%02X]  ", ctx->bytecode[i]);
        else
            fprintf(log, " %02X   ", ctx->bytecode[i]);
    }
}

void print_regs(FILE* log, const stack_elem_t* array, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        fprintf(log, "%cx: %02X  |  ", 'A' + i, (int) array[i]);
        if (i % 2 != 0) fprintf(log, "\n");
    }
}