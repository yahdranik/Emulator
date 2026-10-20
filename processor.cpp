#include "processor.h"
#include <cstdio>

// TODO: опер память, сорс нормальный написать, дамп
//TODO: ошибки по типу, синтаксические, рантайм и тд
// хранение в бинарном виде, листинг
// разделить на две разные проги
// сделать еще один хэдер для общих функций и констант транслятора и процессора
// возвращать SPU_ERROR в функциях



// int main()
// {
//     FILE* log =  fopen("blyat_logs.log", "w");
//     stack_t stack = {};
//     stack_initialise(&stack, 16);

//     FileData bytecode_info = read_file_and_get_data("byte_code.asm");

//     spu_ctx ctx = {};
//     ctx_initialise(&ctx, bytecode_info.count_of_strings, 4, bytecode_info.count_of_strings * 2);

//     get_bytecode(&bytecode_info, &ctx);
//     SPU_DUMP(log, &ctx);
//     processing_bytecode(&ctx, bytecode_info.count_of_strings * 2, &stack);

//     fclose(log);
// }

spu_error_t ctx_initialise(spu_ctx* ctx, size_t bytecode_size)
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

    return SPU_SUCCESS;
}

spu_error_t processing_bytecode(spu_ctx* ctx, int size, stack_t* stack)
{
    int ip = 0;
    spu_error_t (*comands[]) (spu_cmd_args*) = {hlt, push, add, sub, div, out, in, jump, je, jne, pushr, popr, call, ret, pop, mult, sqr};

    ctx->current_ip = ip;

    while(ip < size)
    {
        ctx->current_ip = ip;

        int cmd = (int) ctx->bytecode[ip];
        if (cmd < 0 || cmd >= COUNT_CMDS)
        {
            ctx->error |= SPU_INCORRECT_CMD;
            break;
        }

        spu_cmd_args cmd_args = {ctx, stack, ctx->bytecode[ip + 1], &ip};
        comands[cmd](&cmd_args);

        if (ctx->error) break;
    }

    return SPU_SUCCESS;
}

spu_error_t push(spu_cmd_args* args)
{
    stack_err_t res_push = stack_push(args -> stack, args -> arg);
    if (res_push != STK_SUCCESS) args->ctx->error |= SPU_PUSH_ERROR;

    *(args -> ip) += 2;

    return SPU_SUCCESS;
}

spu_error_t pushr(spu_cmd_args* args)
{
    if (args -> arg < 0 || args -> arg >= COUNT_REGS) {args->ctx->error |= SPU_REG_VALUE_ERROR; return SPU_ERROR;}

    stack_err_t res_push = stack_push(args -> stack, args -> ctx -> regs[(int) args -> arg]);
    if (res_push != STK_SUCCESS) args->ctx->error |= SPU_PUSH_ERROR;

    *(args -> ip) += 2;

    return SPU_SUCCESS;
}

spu_error_t popr(spu_cmd_args* args)
{
    if (args -> arg < 0 || args -> arg >= COUNT_REGS) {args->ctx->error |= SPU_REG_VALUE_ERROR; return SPU_ERROR;}

    stack_err_t res_pop = stack_pop(args -> stack, &(args -> ctx->regs[(int) args -> arg]));

    if (res_pop != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;

    *(args -> ip) += 2;

    return SPU_SUCCESS;
}

spu_error_t pop(spu_cmd_args* args)
{
    stack_elem_t elem_1 = BYTECODE_INIT;
    stack_err_t res_pop = stack_pop(args -> stack, &elem_1);

    if (res_pop != STK_SUCCESS) args->ctx->error |= SPU_POP_ERROR;

    *(args -> ip) += 1;

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

spu_error_t je(spu_cmd_args* args)
{
    if(args->arg > args->ctx->bytecode_size) {args->ctx->error |= SPU_INCORRECT_ARG; return SPU_ERROR;}

    stack_elem_t elem_1 = 0;
    stack_elem_t elem_2 = 0;

    stack_err_t res_pop_1 = stack_pop(args -> stack, &elem_1);
    if (res_pop_1 != STK_SUCCESS)   args->ctx->error |= SPU_POP_ERROR;

    stack_err_t res_pop_2 = stack_pop(args -> stack, &elem_2);
    if (res_pop_2 != STK_SUCCESS)   args->ctx->error |= SPU_POP_ERROR;

    if (is_equal(elem_1,elem_2))    *(args->ip) = (int) args->arg;
    else        *(args->ip) += 2;

    return SPU_SUCCESS;
}

spu_error_t jne(spu_cmd_args* args)
{
    if(args->arg > args->ctx->bytecode_size) {args -> ctx->error |= SPU_INCORRECT_ARG; return SPU_ERROR;}

    stack_elem_t elem_1 = 0;
    stack_elem_t elem_2 = 0;

    stack_err_t res_pop_1 = stack_pop(args -> stack, &elem_1);
    if (res_pop_1 != STK_SUCCESS) args -> ctx -> error |= SPU_POP_ERROR;

    stack_err_t res_pop_2 = stack_pop(args -> stack, &elem_2);
    if (res_pop_2 != STK_SUCCESS) args -> ctx->error |= SPU_POP_ERROR;

    if (!is_equal(elem_1, elem_2)) *(args -> ip) = (int) args -> arg;
    else        *(args -> ip) += 2;

    return SPU_SUCCESS;
}

spu_error_t jump(spu_cmd_args* args)
{
    if(args -> arg > args -> ctx -> bytecode_size) {args -> ctx -> error |= SPU_INCORRECT_ARG; return SPU_ERROR;}

    *(args -> ip) = (int) args -> arg;

    return SPU_SUCCESS;
}

spu_error_t call(spu_cmd_args* args)
{
    stack_err_t res_push = stack_push(args -> ctx -> func_rets, *args -> ip + 2);
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

    spu_cmd_args args_ret = {args -> ctx, args -> stack, ret_adress, args -> ip};

    spu_error_t res_jump = jump(&args_ret);
    if (res_jump == SPU_ERROR) return SPU_ERROR;

    return SPU_SUCCESS;
}

spu_error_t hlt(spu_cmd_args* args)
{
    printf("HLT summoned in %d comand\n", *(args -> ip));
    exit(0);

    return SPU_SUCCESS;
}

void spu_dump(FILE* log, spu_ctx* ctx, const char* file_name, const char* func_name, int line)
{
    int safe_ip = ctx->current_ip;
    if (safe_ip < 0 || safe_ip >= ctx->bytecode_size) safe_ip = 0;

    bytecode_t cur_cmd = ctx->bytecode[safe_ip]; 

    fprintf(log, "code error: %zu\n", ctx->error);
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
                 "      current comand: %lg (%s)"
                 "      bytecode size: %d",
                 ctx, ctx->bytecode, ctx->regs, ctx->func_rets, safe_ip, 
                 cur_cmd, CMD_NAME(cur_cmd), ctx->bytecode_size);

    fflush(log);
    fprintf(log, "\n\nBytecode:\n");
    print_bytecode(log, ctx);

    fprintf(log, "\n\nRegisters:\n");
    print_regs(log, ctx->regs, 4);
    
    fflush(log);
}


void print_bytecode(FILE* log, const spu_ctx* ctx)
{
    for (int i = 0; i < ctx->bytecode_size; i++)
    {
        if (i == ctx->current_ip) fprintf(log, "[%02X]", (int) ctx->bytecode[i]);
        else                      fprintf(log, " %02X ", (int) ctx->bytecode[i]);
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



// spu_error_t get_bytecode(FileData* bytecode_info, spu_ctx* ctx)
// {
//     char* ptr =  bytecode_info -> original_text;
//     int ip = 0;

//     for (int i = 0; i < bytecode_info->size_of_file; i++)
//     {
//         if (bytecode_info -> original_text[i] == '\n')
//         {
//             bytecode_info -> original_text[i] = '\0';

//             bytecode_t cmd = BYTECODE_INIT;
//             bytecode_t arg = BYTECODE_INIT;

//             sscanf(ptr, BC_SP BC_SP, &cmd, &arg);

//             if (arg != BYTECODE_INIT)
//             {
//                 ctx->bytecode[ip] = cmd;
//                 ctx->bytecode[ip + 1] = arg;
//                 ip += 2;
//             }

//             else 
//             {
//                 ctx->bytecode[ip] = cmd;
//                 ip += 1;
//             }
            
//             ptr = &bytecode_info->original_text[i + 1];
//         }
//     }
//     return SPU_SUCCESS;
// }