#include "translator.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

int main()
{
    FileData source_info = read_file_and_get_data("TESTS/square.asm");
        
    spu_ctx ctx = {};
    ctx_initialise(&ctx, (int) source_info.count_of_strings * 9);

    spu_label_t* labels = (spu_label_t*) calloc(source_info.count_of_strings, sizeof(spu_label_t)); 
    if (labels == NULL) {ctx.error |= SPU_NOT_MEMORY; return SPU_ERROR;}

    translate_to_bytecode(&source_info, &ctx, labels, 1);
    translate_to_bytecode(&source_info, &ctx, labels, 2);

    FILE* code = fopen("TESTS/byte_code.asm", "w");
    FILE* code_bin = fopen("TESTS/bytecode.bin", "wb");
    FILE* list = fopen("TOOLS/square_listing.list", "w");

    spu_header_t header = {};

    header.bytecode_size = ctx.bytecode_size;
    header.magic = DRM_MAGIC;
    header.version = SPU_VERSION;

    fwrite(&header, sizeof(spu_header_t), 1, code_bin);
    fwrite(ctx.bytecode, sizeof(bytecode_t), ctx.bytecode_size, code_bin);

    print_list(list, &ctx, &source_info);

    fclose(code);
    fclose(code_bin);
    fclose(list);

    ctx_destroy(&ctx);
    free(labels);
}

spu_error_t translate_to_bytecode(FileData* source_info, spu_ctx* ctx, spu_label_t* labels, int pass_number)
{
    bool is_comment = 0;
    size_t label_number = 0;
    bool   is_line_end  = 0;
    int    ip   = 0;
    char*  ptr  = source_info->original_text;
    char*  text = source_info->original_text;

    for (size_t i = 0; i < source_info->size_of_file; i++)
    {
        char c = text[i];

        FIRST_PASS
        {
            if (c == ';')    { is_comment = 1; }
            is_line_end = (c == '\n');
            if (is_line_end) { text[i] = '\0'; is_comment = 0; }
            if (is_comment)  { text[i] = '\0'; }
        }

        SECOND_PASS
        {
            is_line_end = (c == '\0');
        }

        if (!is_line_end)
            continue;

        source_t label = {}, cmd = {}, arg = {};

        if (strchr(ptr, ':') != NULL)
            sscanf(ptr, "%63[^:]", label);
        else
            sscanf(ptr, "%63s %63s", cmd, arg);

        FIRST_PASS
        {
            if (label[0] != '\0')    put_one_label(labels, label, ip, &label_number);
            if (arg[0] != '\0')      ip += 9;
            else if (cmd[0] != '\0') ip += 1;
        }

        SECOND_PASS
        {
            if (cmd[0] != '\0')
            {
                bytecode_t converted = convert_one_comand(cmd);
                if (!is_equal(converted, BYTECODE_INIT)) comand_processing_to_bytecode(ctx, arg, converted,
                                                &ip, labels, source_info->count_of_strings);
            }
        }

        ptr = &text[i + 1];
    }

    ctx->bytecode_size = ip;

    return SPU_SUCCESS;
}

spu_error_t comand_processing_to_bytecode(spu_ctx* ctx, char* arg, bytecode_t cmd, int* ip, spu_label_t* labels, size_t count_strings)
{
    uint8_t mode = NO_ARG;
    stack_elem_t arg_value = 0.0;

    if (arg[0] != '\0' && (cmd == PUSH || cmd == POP))
    {
        if (arg[0] == '[') 
        {
            char inside[64] = {};
            sscanf(arg, "[%63[^]]]", inside);

            if (is_number(inside)) 
            {
                mode = RAM_BYTE;
                arg_value = strtod(inside, NULL);
            } 

            else if (inside[0] >= 'A' && inside[0] <= 'P') 
            {
                mode = (RAM_BYTE | REG_BYTE);
                arg_value = (double)(inside[0] - 'A');
            }
        }

        else if (is_number(arg) == 1) 
        {
            mode = CONST_BYTE;
            arg_value = strtod(arg, NULL);
        }

        else if (arg[0] >= 'A' && arg[0] <= 'P') 
        {
            mode = REG_BYTE;
            arg_value = (double)(arg[0] - 'A');
        }

        else
        {
            ctx->error |= SPU_INCORRECT_ARG;
            return SPU_ERROR;
        }
    }

    else if (arg[0] != '\0' && (cmd == JUMP || cmd == JE || cmd == JNE || cmd == CALL))
    {
        if      (cmd == JE)    { mode = J_EQ_BYTE;    cmd = JUMP; }
        else if (cmd == JNE)   { mode = J_NEQ_BYTE;   cmd = JUMP; }
        else if (cmd == JUMP)  { mode = SIMPL_J_BYTE; cmd = JUMP; } 
        else if (cmd == CALL)    mode = SIMPL_J_BYTE;

        int adress = find_adress(labels, count_strings, arg);
        if (adress == -1) { ctx->error |= SPU_INCORRECT_LABEL; return SPU_ERROR; }

        arg_value = (double) adress;
    }

    uint8_t instruction = ((uint8_t)cmd << 3) | mode; 
    ctx->bytecode[*ip] = instruction;

    if (mode != NO_ARG || cmd == JUMP || cmd == CALL) 
    {
        memcpy(&ctx->bytecode[*ip + 1], &arg_value, sizeof(stack_elem_t)); 
        *ip += 9;
    }

    else 
    {
        *ip += 1;
    }

    return SPU_SUCCESS;
}

int is_number(const char* arg)
{
    if (arg == NULL || *arg == '\0')
        return 0;

    char* end = NULL;
    strtod(arg, &end);

    return end != arg && *end == '\0';
}

spu_error_t put_one_label(spu_label_t* labels, char* label, int ip, size_t* label_number)
{
    strncpy(labels[*label_number].name, label, sizeof(labels[0].name) - 1);
    labels[*label_number].name[sizeof(labels[0].name) - 1] = '\0';
    labels[*label_number].adress = ip;

    *label_number += 1;

    return SPU_SUCCESS;
}

int find_adress(spu_label_t* labels, size_t size, source_t label)
{
    if (label[0] == '\0') return -1;

    for (size_t i = 0; i < size; i++)
    {
        if (strcmp(labels[i].name, label) == 0)
        {
            return labels[i].adress;
        }
    }

    return -1;
}

bytecode_t convert_one_comand(source_t cmd)
{
    if (strcmp(cmd, "PUSH")  == 0) {return PUSH;}
    if (strcmp(cmd, "PUSHr") == 0) {return PUSH;}
    if (strcmp(cmd, "POP")   == 0) {return POP;}
    if (strcmp(cmd, "POPr")  == 0) {return POP;}
    if (strcmp(cmd, "ADD")   == 0) {return ADD;}
    if (strcmp(cmd, "SUB")   == 0) {return SUB;}
    if (strcmp(cmd, "DIV")   == 0) {return DIV;}
    if (strcmp(cmd, "OUT")   == 0) {return OUT;}
    if (strcmp(cmd, "JE")    == 0) {return JE;}
    if (strcmp(cmd, "JNE")   == 0) {return JNE;}
    if (strcmp(cmd, "IN")    == 0) {return IN;}
    if (strcmp(cmd, "HLT")   == 0) {return HLT;}
    if (strcmp(cmd, "CALL")  == 0) {return CALL;}
    if (strcmp(cmd, "RET")   == 0) {return RET;}
    if (strcmp(cmd, "JUMP")  == 0) {return JUMP;}
    if (strcmp(cmd, "POP")   == 0) {return POP;}
    if (strcmp(cmd, "SQR")   == 0) {return SQR;}
    if (strcmp(cmd, "MULT")  == 0) {return MULT;}
    if (strcmp(cmd, "DRAW")  == 0) {return DRAW;}

    return BYTECODE_INIT;
}

spu_error_t ctx_destroy(spu_ctx* ctx)
{
    if (ctx == NULL) return SPU_ERROR;

    free(ctx->bytecode);
    ctx->bytecode = NULL;

    stack_destroy(ctx->func_rets);
    free(ctx->func_rets);

    ctx->error = SPU_ALL_OK;

    return SPU_SUCCESS;
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

    return SPU_SUCCESS;
}

void print_list(FILE* list, spu_ctx* ctx, FileData* source_info)
{
    fprintf(list, "======================================================\n");
    fprintf(list, "SPU Translator Listing (version %d)\n", SPU_VERSION);
    fprintf(list, "======================================================\n\n");
    
    fprintf(list, "%-6s | %-28s | %s\n", "Addr", "Bytes", "Command");
    fprintf(list, "-------|------------------------------|-----------------\n");

    int ip = 0;
    
    while (ip < ctx->bytecode_size)
    {
        uint8_t cmd = ctx->bytecode[ip];
        uint8_t opcode = cmd >> 3;
        uint8_t mask = cmd & 0x07;
        int size = sizeof(uint8_t);

        if (opcode == PUSH || opcode == POP || opcode == JUMP || opcode == CALL)
        {
            size = sizeof(uint8_t) + sizeof(stack_elem_t);
        }

// CMD IP ---------------------------------------------------------------------------------

        fprintf(list, "0x%04X | ", ip);

// OPCODE AND ARG -------------------------------------------------------------------------

        for (int j = 0; j < size; j++) 
        {
            fprintf(list, "%02X ", ctx->bytecode[ip + j]);
        }

        for (int j = size; j <= sizeof(stack_elem_t); j++) 
        {
            fprintf(list, "   ");
        }

        fprintf(list, "  ");

// CMD NAME --------------------------------------------------------------------------------

        fprintf(list, "| %-6s", CMD_NAME(opcode));

        if (size == sizeof(uint8_t) + sizeof(stack_elem_t))
        {
            if ((mask & RAM_BYTE) == RAM_BYTE) {fprintf(list, "[");}

            if ((mask & CONST_BYTE) == CONST_BYTE)
            {
                stack_elem_t arg = 0;
                if (ip + 1 + sizeof(stack_elem_t) <= ctx->bytecode_size) 
                {
                    memcpy(&arg, &ctx->bytecode[ip + 1], sizeof(stack_elem_t));
                    fprintf(list, " %lg", arg);
                }
            }
            
            else if ((mask & REG_BYTE) == REG_BYTE)
            {
                stack_elem_t arg = 0;
                if (ip + 1 + sizeof(stack_elem_t) <= ctx->bytecode_size) 
                {
                    memcpy(&arg, &ctx->bytecode[ip + 1], sizeof(stack_elem_t));
                    fprintf(list, " %cx", 'A' + (int) arg);
                }
            }

           if ((mask & RAM_BYTE) == RAM_BYTE) {fprintf(list, "]");}
        }

        ip += size;
        fprintf(list, "\n");
    }
}