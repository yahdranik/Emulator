#include "translator.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

int main()
{
    FileData source_info = read_file_and_get_data("asm_source.asm");
        
    spu_ctx ctx = {};
    ctx_initialise(&ctx, source_info.count_of_strings * 2); //общий хэдер 

    spu_label_t* labels = (spu_label_t*) calloc(source_info.count_of_strings, sizeof(spu_label_t)); 

    translate_to_bytecode(&source_info, &ctx, labels, 1);
    translate_to_bytecode(&source_info, &ctx, labels, 2);

    FILE* code = fopen("byte_code.asm", "w");
    FILE* code_bin = fopen("bytecode.bin", "wb");
    print_into_file(ctx.bytecode, ctx.bytecode_size, code, code_bin);
    fclose(code);
    fclose(code_bin);
}

spu_error_t translate_to_bytecode(FileData* source_info, spu_ctx* ctx, spu_label_t* labels, int pass_number)
{
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
            if (c == ';') { text[i] = '\0'; continue; }
            is_line_end = (c == '\n');
            if (is_line_end) text[i] = '\0';
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
            if (arg[0] != '\0')      ip += 2;
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

    ctx->bytecode_size = ip + 1;

    return SPU_SUCCESS;
}

spu_error_t comand_processing_to_bytecode(spu_ctx* ctx, char* arg, bytecode_t cmd, int* ip, spu_label_t* labels, size_t count_strings)
{
    if (arg[0] != '\0')
    {
        ctx->bytecode[*ip] = cmd;

        if (is_number(arg) == 1)
        {
            char* end = NULL;
            double value = strtod(arg, &end);

            if (end == arg || *end != '\0') return SPU_ERROR;

            ctx->bytecode[*ip + 1] = value;
        }

        else
        {
            if (is_equal(cmd, JE) || is_equal(cmd, JNE) || is_equal(cmd, JUMP) || is_equal(cmd, CALL))
            {
                int adress = find_adress(ctx, count_strings, arg);
                ctx->bytecode[*ip + 1] = adress;
            }

            else if (is_equal(cmd, PUSHr) || is_equal(cmd, POPr))
            {
                if (arg[0] >= 'A' && arg[0] <= 'P')
                    ctx->bytecode[*ip + 1] = arg[0] - 'A';
            }
        }

        *ip += 2;
    }

    else
    {
        ctx->bytecode[*ip] = cmd;
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
    if (strcmp(cmd, "ADD")   == 0) {return ADD;}
    if (strcmp(cmd, "SUB")   == 0) {return SUB;}
    if (strcmp(cmd, "DIV")   == 0) {return DIV;}
    if (strcmp(cmd, "OUT")   == 0) {return OUT;}
    if (strcmp(cmd, "JE")    == 0) {return JE;}
    if (strcmp(cmd, "JNE")   == 0) {return JNE;}
    if (strcmp(cmd, "IN")    == 0) {return IN;}
    if (strcmp(cmd, "PUSHr") == 0) {return PUSHr;}
    if (strcmp(cmd, "POPr")  == 0) {return POPr;}
    if (strcmp(cmd, "HLT")   == 0) {return HLT;}
    if (strcmp(cmd, "CALL")  == 0) {return CALL;}
    if (strcmp(cmd, "RET")   == 0) {return RET;}
    if (strcmp(cmd, "JUMP")  == 0) {return JUMP;}
    if (strcmp(cmd, "POP")   == 0) {return POP;}
    if (strcmp(cmd, "SQR")   == 0) {return SQR;}
    if (strcmp(cmd, "MULT")  == 0) {return MULT;}

    return BYTECODE_INIT;
}

void print_into_file(bytecode_t* bytecode, size_t size, FILE* code, FILE* code_bin)
{
    fwrite(bytecode, sizeof(bytecode_t), size, code_bin);

    size_t i = 0;
    while (i < size)
    {
        if (is_equal(bytecode[i], PUSH) || is_equal(bytecode[i], PUSHr) || is_equal(bytecode[i], POPr) 
         || is_equal(bytecode[i], JE)   || is_equal(bytecode[i], JNE)   || is_equal(bytecode[i], CALL))
        {
            fprintf(code, BC_SP BC_SP "\n", bytecode[i], bytecode[i + 1]);
            i += 2;
        }
        else 
        {
            fprintf(code, BC_SP "\n", bytecode[i]);
            i += 1;
        }
    }
}

spu_error_t ctx_destroy(spu_ctx* ctx)
{
    if (ctx == NULL) return SPU_ERROR;

    free(ctx->bytecode);
    ctx->bytecode = NULL;

    ctx->error = SPU_ALL_OK;

    return SPU_SUCCESS;
}

int hash_labels(const char* variable)              //Used hash algorithm - FNV:    https://www.isthe.com/chongo/tech/comp/fnv/
{
    uint32_t hash = FNV_BASIS;
    for (size_t i = 0; i < strlen(variable); i++)
    {
        hash ^= variable[i];
        hash *= FNV_PRIME;
    }

    return hash % 1024;
}