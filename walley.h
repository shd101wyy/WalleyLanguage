//
//  walley.h
//  walley
//
//  Created by WangYiyi on 4/28/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_walley_h
#define walley_walley_h
#include "vm.h"

Object * Walley_Run_File_for_VM(char * file_name,
                                Instructions * insts,
                                Variable_Table * vt,
                                Environment * env,
                                MacroTable * mt);
void Walley_Run_Compiled_File(char * file_name);

/*
 read ints and return instructions
 0: fail
 1: success
 */
int32_t read_ints (const char* file_name, uint16_t ** instructions, int32_t * len){
    FILE* file = fopen (file_name, "r");
    if(!file){
        printf("Failed to read file\n");
        return 0;
    }
    int32_t num = 0;
    uint32_t i = 0;
    uint32_t length = 0;
    while (fscanf(file, "%x ", &num) > 0)
    {
        switch (i){
            case 0:
                length = num << 16;
                break;
            case 1:
                length = length | num;
                *len = length; // get length
                (*instructions) = (uint16_t*)malloc(sizeof(uint16_t) * length); // init instructions
                break;
            default:
                (*instructions)[i - 2] = num;
                break;
        }
        i++;
    }
    fclose (file);
    return 1;
}

// repl
void Walley_Repl(){
    // init walley
    Walley_init();
    
    Lexer * p;
    Object * o;
    
    //Instructions * insts = Insts_init();
    //Variable_Table * vt = VT_init();
    //Environment * env = createEnvironment();
    //MacroTable * mt = MT_init();

    Instructions * insts = GLOBAL_INSTRUCTIONS;
    Variable_Table * vt = GLOBAL_VARIABLE_TABLE;
    Environment * env = GLOBAL_ENVIRONMENT;
    MacroTable * mt = GLOBAL_MACRO_TABLE;
    
    // run walley_core.wa
    Walley_Run_File_for_VM("/usr/local/lib/walley/walley_core.wa", // assume is this folder
                           insts,
                           vt,
                           env,
                           mt);
    
    int32_t run_eval = true;
    
    //Environment * env = NULL;
    //int run_eval = false;
    
    Object * v;
    

    char buffer[512];
    char * s;
    while (1) {
        fputs("walley> ", stdout);
        fgets(buffer, 512, stdin);
        p = lexer(buffer);
        o = parser(p);
        
        
        // compile
        v = compiler_begin(insts,
                       o,
                       vt,
                       NULL,
                       NULL,
                       run_eval,
                       env,
                       mt);
        
        /*
         // print parser
        s = to_string(o);
        printf("%s\n", s);
        free(s);
        */
        
        s = to_string(v);
        printf("\n        %s\n", (s));
        free(s); // need to free that value
        
        // printf("\nuse count %d\n", v->use_count);
        Object_free(v); // free accumulator
        

        // parser will be freed after compiler_begin finished
        //Object_free(o); // free parser
        
        
#if WALLEY_DEBUG
        Walley_Debug(v);
#endif
        
        
    }
}

/*
    suppose run .wa / .wac file
 */
void Walley_Run_File(char * file_name){
    
    // check .wac file
    uint32_t file_name_length = (uint32_t)strlen(file_name);
    if (file_name_length >= 5 &&
        file_name[file_name_length - 1] == 'c' &&
        file_name[file_name_length - 2] == 'a' &&
        file_name[file_name_length - 3] == 'w' &&
        file_name[file_name_length - 4] == '.') {
        return Walley_Run_Compiled_File(file_name);
    }
    
    // read content from file
    FILE* file = fopen(file_name,"r");
    if(file == NULL)
    {
        printf("Failed to read file %s\n", file_name);
        return; // fail to read
    }
    
    fseek(file, 0, SEEK_END);
    int64_t size = ftell(file);
    rewind(file);
    
    char* content = calloc(size + 1, 1);
    
    fread(content,1,size,file);
    
    fclose(file); // 不知道要不要加上这个
    
    // init walley
    Walley_init();
    
    Lexer * p;
    Object * o;
    
/*
    Instructions * insts = Insts_init();
    Variable_Table * vt = VT_init();
    Environment * env = createEnvironment();
    MacroTable * mt = MT_init();
*/
    
    Instructions * insts = GLOBAL_INSTRUCTIONS;
    Variable_Table * vt = GLOBAL_VARIABLE_TABLE;
    Environment * env = GLOBAL_ENVIRONMENT;
    MacroTable * mt = GLOBAL_MACRO_TABLE;
    
    // run walley_core.wa
    Walley_Run_File_for_VM("/usr/local/lib/walley/walley_core.wa", // assume is this folder
                           insts,
                           vt,
                           env,
                           mt);

    int32_t run_eval = true;
    
    //Environment * env = NULL;
    //int run_eval = false;
    
    //Object * v;
    
    p = lexer(content);
    o = parser(p);
    
    // compile
    /*v = */compiler_begin(insts,
                       o,
                       vt,
                       NULL,
                       NULL,
                       run_eval,
                       env,
                       mt);
    
    free(content);
    return;
}
/*
 *  run Compiled wac file
 *  first 64 bytes, length of CONSTANT_TABLE_INSTRUCTIONS
 *  second 64 bytes, length of insts
 *  0000 0000 0000 0000 length of CONSTANT_TABLE_INSTRUCTIONS
 *  0000 0000 0000 0000 length of insts
 *  0000                reserved
 */
void Walley_Run_Compiled_File(char * file_name){
    FILE* file = fopen (file_name, "r");
    if(!file){
        printf("Failed to read file\n");
        return;
    }
    
    // init walley
    Walley_init();
    
    Instructions * insts = NULL; //  = GLOBAL_INSTRUCTIONS;
    //Variable_Table * vt = GLOBAL_VARIABLE_TABLE;
    Environment * env = GLOBAL_ENVIRONMENT;
    //MacroTable * mt = GLOBAL_MACRO_TABLE;
    
    
    uint16_t num = 0;
    uint32_t i = 0;
    uint64_t constant_table_insts_length = 0;
    uint64_t insts_length = 0;
    while (fscanf(file, "%hx ", &num) > 0)
    {
        // first 64 bytes CONSTANT_TABLE_INSTRUCTIONS length
        if (i < 4) {
            constant_table_insts_length |=   (uint64_t)(num << ((3 - i) * 16));
        }
        else if (i < 8){
            insts_length |= (uint64_t)(num << ((3 - (i - 4)) * 16));
        }
        else if (i == 8){
            // init insts
            insts = (Instructions*)malloc(sizeof(Instructions));
            insts->length = 0;
            insts->size = insts_length + 1;
            insts->array = (uint16_t *) malloc(sizeof(uint16_t) * (insts_length + 1));
            insts->start_pc = 0;
        }
        else{
            if (i - 9 < constant_table_insts_length) {
                Insts_push(CONSTANT_TABLE_INSTRUCTIONS, num);
            }
            else{
                Insts_push(insts, num);
            }
        }
        i++;
    }
    fclose (file);
    
    //printf("%llu %llu\n", constant_table_insts_length, insts_length);
    
    VM(insts->array, 0, insts->length, env, NULL, NULL);
    return;
}

/*
 suppose run .wa file
 */

Object * Walley_Run_File_for_VM(char * file_name,
                                Instructions * insts,
                                Variable_Table * vt,
                                Environment * env,
                                MacroTable * mt){
    // read content from file
    FILE* file = fopen(file_name,"r");
    if(file == NULL)
    {
        printf("Failed to read file %s\n", file_name);
        return GLOBAL_NULL; // fail to read
    }
    
    fseek(file, 0, SEEK_END);
    int64_t size = ftell(file);
    rewind(file);
    
    char* content = calloc(size + 1, 1);
    
    fread(content,1,size,file);
    
    fclose(file); // 不知道要不要加上这个
    
    Lexer * p;
    Object * o;
    
    int32_t run_eval = true;
    
    //Environment * env = NULL;
    //int run_eval = false;
    
    
    p = lexer(content);
    o = parser(p);
    
    // compile
    Object * return_value = compiler_begin(insts,
                           o,
                           vt,
                           NULL,
                           NULL,
                           run_eval,
                           env,
                           mt);
    
    free(content);
    return return_value;
}
/*
    run single string
 */
Object * Walley_RunString(char * input_string){
    Lexer * p;
    Object * o;
    
    int32_t run_eval = true;
    
    p = lexer(input_string);
    o = parser(p);
    
    // compile
    Object * return_value = compiler_begin(GLOBAL_INSTRUCTIONS,
                                           o,
                                           GLOBAL_VARIABLE_TABLE,
                                           NULL,
                                           NULL,
                                           run_eval,
                                           GLOBAL_ENVIRONMENT,
                                           GLOBAL_MACRO_TABLE);
    
    return return_value;

}

// compile to .wac file
void Walley_Compile(char * file_name){

    COMPILATION_MODE = 1; // if under compilation mode, no print necessary

    // read content from file
    FILE* file = fopen(file_name,"r");
    if(file == NULL){
        printf("Failed to read file %s\n", file_name);
        return; // fail to read
    }
    
    fseek(file, 0, SEEK_END);
    int64_t size = ftell(file);
    rewind(file);
    
    char* content = calloc(size + 1, 1);
    
    fread(content,1,size,file);
    
    fclose(file); // 不知道要不要加上这个
    
    // init walley
    Walley_init();
    
    Lexer * p;
    Object * o;
    
    /*
     Instructions * insts = Insts_init();
     Variable_Table * vt = VT_init();
     Environment * env = createEnvironment();
     MacroTable * mt = MT_init();
     */
    
    Instructions * insts = GLOBAL_INSTRUCTIONS;
    Variable_Table * vt = GLOBAL_VARIABLE_TABLE;
    Environment * env = GLOBAL_ENVIRONMENT;
    MacroTable * mt = GLOBAL_MACRO_TABLE;
    
    // run walley_core.wa
    Walley_Run_File_for_VM("/usr/local/lib/walley/walley_core.wa", // assume is this folder
                           insts,
                           vt,
                           env,
                           mt);
    
    int32_t run_eval = true;
    
    //Environment * env = NULL;
    //int run_eval = false;
    
    //Object * v;
    
    p = lexer(content);
    o = parser(p);
    
    // compile
    /*v = */compiler_begin(insts,
                           o,
                           vt,
                           NULL,
                           NULL,
                           run_eval,
                           env,
                           mt);
    
    free(content);
    
    //printf("INSTS LENGTH %llu\n", insts->length);
    //printf("CONSTANT TABLE LENGTH %llu\n", CONSTANT_TABLE_INSTRUCTIONS->length);
    
    char file_name_buffer[64];
    char inst_buffer[64];
    strcpy(file_name_buffer, file_name);
    strcat(file_name_buffer, "c");
    file = fopen(file_name_buffer, "w");
    uint64_t i;
    
    // first 64 bytes, length of CONSTANT_TABLE_INSTRUCTIONS
    sprintf(inst_buffer, "%04x ", (uint16_t)((CONSTANT_TABLE_INSTRUCTIONS->length & 0xFFFF000000000000) >> 48));
    fputs(inst_buffer, file);
    sprintf(inst_buffer, "%04x ", (uint16_t)((CONSTANT_TABLE_INSTRUCTIONS->length & 0x0000FFFF00000000) >> 32));
    fputs(inst_buffer, file);
    sprintf(inst_buffer, "%04x ", (uint16_t)((CONSTANT_TABLE_INSTRUCTIONS->length & 0x00000000FFFF0000) >> 16));
    fputs(inst_buffer, file);
    sprintf(inst_buffer, "%04x ", (uint16_t)((CONSTANT_TABLE_INSTRUCTIONS->length & 0x000000000000FFFF) >> 0));
    fputs(inst_buffer, file);

    // seconds 64 bytes, length of INSTRUCTIONS
    sprintf(inst_buffer, "%04x ", (uint16_t)((insts->length & 0xFFFF000000000000) >> 48));
    fputs(inst_buffer, file);
    sprintf(inst_buffer, "%04x ", (uint16_t)((insts->length & 0x0000FFFF00000000) >> 32));
    fputs(inst_buffer, file);
    sprintf(inst_buffer, "%04x ", (uint16_t)((insts->length & 0x00000000FFFF0000) >> 16));
    fputs(inst_buffer, file);
    sprintf(inst_buffer, "%04x ", (uint16_t)((insts->length & 0x000000000000FFFF) >> 0));
    fputs(inst_buffer, file);
    
    // 0000 0000 0000 0000 length of CONSTANT_TABLE_INSTRUCTIONS
    // 0000 0000 0000 0000 length of insts
    // 0000                reserved
    
    fputs("0000 ", file);
    
    // save CONSTANT TABLE INSTRUCTIONS
    for (i = 0; i < CONSTANT_TABLE_INSTRUCTIONS->length; i++) {
        sprintf(inst_buffer, "%04x ", CONSTANT_TABLE_INSTRUCTIONS->array[i]);
        fputs(inst_buffer, file);
    }
    // save INSTRUCTIONS
    for (i = 0; i < insts->length; i++) {
        sprintf(inst_buffer, "%04x ", insts->array[i]);
        fputs(inst_buffer, file);
    }
    fclose(file);
    
    return;
}


#endif



