#include <stdio.h>
#include "processor.h"
#include "translator.h"

// int main()
// {
//     FILE* log =  fopen("blin_logs.log", "w");
//     FileData source_info = read_file_and_get_data("square.asm");

//     stack_t stack = {};
//     stack_initialise(&stack, 16);

//     spu_ctx ctx = {};
//     ctx_initialise(&ctx, source_info.count_of_strings * 2);

// // Translator -------------------------------------------------------------------------------------------------

//     int labels[1024] = {}; 
//     translate_to_bytecode(&source_info, &ctx, labels, 1);
//     translate_to_bytecode(&source_info, &ctx, labels, 2);

//     FILE* code = fopen("byte_code.asm", "w");
//     FILE* code_bin = fopen("bytecode.bin", "wb");
//     print_into_file(ctx.bytecode, ctx.bytecode_size, code, code_bin);
//     fclose(code);
//     fclose(code_bin);

// // Processor ---------------------------------------------------------------------------------------------------

//     SPU_DUMP(log, &ctx);
//     processing_bytecode(&ctx, ctx.bytecode_size, &stack);
//     fclose(log);
// }