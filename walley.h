//
//  walley.h
//  walley
//
//  Created by WangYiyi on 4/28/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//
#include "vm.h"
#include "walley_core.h" // load walley_core.h (from walley_core.wa)
#ifndef walley_walley_h
#define walley_walley_h
Object * Walley_RunString(char * input_string);
/*
 *
 * switch working directory to the directory of run_file
 *
 */
void SwitchWorkingDirectory(char * working_path){
#ifdef WIN32   // windows
    int32_t i = (uint32_t)strlen(working_path) - 1;
    for (; i >= 0; i--) {
        if (working_path[i] == '\\') {
            working_path[i] = 0;
            break;
        }
        working_path[i] = 0;
    }
    //printf("working_path %s\n", working_path);
    // now abs_path is the folder
	_chdir(working_path); // change working directory

#else      // .nix
    int32_t i = (uint32_t)strlen(working_path) - 1;
    for (; i >= 0; i--) {
        if (working_path[i] == '/') {
            working_path[i] = 0;
            break;
        }
        working_path[i] = 0;
    }
    //printf("working_path %s\n", working_path);
    // now abs_path is the folder
    if(chdir(working_path) != 0){ // change working directory.
        printf("walley.h ERROR: Failed to change working_path\n");
        
    }
    //printf("Current Working Directory %s\n", getcwd(NULL, 0));
#endif
    
}

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
    
    GLOBAL_PUSH_SAVE_TO_VT = false; // GLOBAL_PUSH的时候不用把变量名字加入vt
    
    //Instructions * insts = Insts_init();
    //Variable_Table * vt = VT_init();
    //Environment * env = createEnvironment();
    //MacroTable * mt = MT_init();

    Instructions * insts = GLOBAL_INSTRUCTIONS;
    Variable_Table * vt = GLOBAL_VARIABLE_TABLE;
    Environment * env = GLOBAL_ENVIRONMENT;
    MacroTable * mt = GLOBAL_MACRO_TABLE;
    
    /*
     * todo: windows platform
     *
     */
    // run walley_core.wa (walley_core.h)
    //Walley_RunString(WALLEY_CORE_CONTENT);
    
    
    int32_t run_eval = true;
    //Environment * env = NULL;
    //int run_eval = false;
    
    Object * v;
    char * buffer = NULL;
    size_t n;
    char * s;
    while (1) {
        fputs("walley> ", stdout);
        ssize_t read_n = getline(&buffer, &n, stdin);
        if(read_n > 0 && buffer[read_n - 1] == '\n'){
            buffer[read_n - 1] = '\0';
        }
        p = lexer(buffer);
        if (p == NULL) { // parenthesis doesn't match
            char another_buffer[512];
            strcpy(another_buffer, buffer);
            while (1) {
                fputs("        ", stdout);
                if(fgets(buffer, 512, stdin) == NULL){
                    printf("stdin ERROR: Failed to read from stdin\n");
                }
                strcat(another_buffer, buffer);
                p = lexer(another_buffer);
                if (p != NULL) {
                    break;
                }
            }
        }
        o = parser(p);
                
        // compile
        v = compiler_begin(insts,
                       o,
                       vt,
                       NULL,
                       NULL,
                       run_eval,
                       env,
                       mt,
                       GLOBAL_MODULE);
        
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
    
    // get abs_path of file_name
    char abs_path[256];
    char working_path[256];
// get absolute path
#ifdef WIN32
	GetFullPathName((TCHAR*)file_name, 256, (TCHAR*)abs_path, NULL); // I don't know is this correct
#else
    if(!realpath(file_name, abs_path)){
        printf("walley.h ERROR: Failed to read file %s\n", file_name);
        return;
    }
#endif 
	strcpy(working_path, abs_path);
    // change working directory
    SwitchWorkingDirectory(working_path);
    
    
    // check .wac file
    uint32_t file_name_length = (uint32_t)strlen(abs_path);
    if (file_name_length >= 5 &&
        abs_path[file_name_length - 1] == 'c' &&
        abs_path[file_name_length - 2] == 'a' &&
        abs_path[file_name_length - 3] == 'w' &&
        abs_path[file_name_length - 4] == '.') {
        return Walley_Run_Compiled_File(abs_path);
    }
    
    // read content from file
    FILE* file = fopen(abs_path,"r");
    if(file == NULL)
    {
        printf("Failed to read file %s\n", abs_path);
        return; // fail to read
    }
    
    fseek(file, 0, SEEK_END);
    int64_t size = ftell(file);
    rewind(file);
    
    char* content = (char*)calloc(size + 1, 1);
    
    size_t result = fread(content,1,size,file);
    if (result != size) {
        printf("walley.h ERROR: Failed to read file %s\n", abs_path);
        free(content);
        return;
    }
    
    fclose(file); // 不知道要不要加上这个
    
    // init walley
    Walley_init();
    
    Lexer * p;
    Object * o;
    
    GLOBAL_PUSH_SAVE_TO_VT = false; // GLOBAL_PUSH的时候不用把变量名字加入vt
    
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
    
    
    // run walley_core.wa (walley_core.h)
    Walley_RunString(WALLEY_CORE_CONTENT);
    
    int32_t run_eval = true;
    
    //Environment * env = NULL;
    //int run_eval = false;
    
    //Object * v;
    
    p = lexer(content);
    if (p == NULL) {
        printf("ERROR: parentheses () num doesn't match");
    }
    o = parser(p);
    
    // compile
    /*v = */compiler_begin(insts,
                       o,
                       vt,
                       NULL,
                       NULL,
                       run_eval,
                       env,
                       mt,
                       GLOBAL_MODULE);
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
    // Walley_init();
    Walley_init_constants();
    
    GLOBAL_PUSH_SAVE_TO_VT = true; // GLOBAL_PUSH的时候用把变量名字加入vt
    
    Instructions * insts = NULL; //  = GLOBAL_INSTRUCTIONS;
    GLOBAL_INSTRUCTIONS = NULL;
    //Variable_Table * vt = GLOBAL_VARIABLE_TABLE;
    GLOBAL_VARIABLE_TABLE = NULL;
    Environment * env = createEnvironment();
    //MacroTable * mt = GLOBAL_MACRO_TABLE;
    GLOBAL_MACRO_TABLE = NULL;
    
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
    
    VM(insts, 0, insts->length, env, NULL, NULL,
       NULL, GLOBAL_NULL, NULL); /* TODO add module */
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
    
    char* content = (char*)calloc(size + 1, 1);
    
    size_t result = fread(content,1,size,file);
    if (result != size) {
        printf("walley.h ERROR: Failed to read file %s\n", file_name);
        free(content);
        return GLOBAL_NULL;
    }
    
    fclose(file); // 不知道要不要加上这个
    
    Lexer * p;
    Object * o;
    
    int32_t run_eval = true;
    
    //Environment * env = NULL;
    //int run_eval = false;
    
    
    p = lexer(content);
    if (p == NULL) {
        printf("ERROR: parentheses () num doesn't match");
    }
    o = parser(p);
    
    // compile
    Object * return_value = compiler_begin(insts,
                           o,
                           vt,
                           NULL,
                           NULL,
                           run_eval,
                           env,
                           mt,
                           GLOBAL_MODULE);
    
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
    if (p == NULL) {
        printf("ERROR: parentheses () num doesn't match");
    }
    o = parser(p);
    
    // compile
    Object * return_value = compiler_begin(GLOBAL_INSTRUCTIONS,
                                           o,
                                           GLOBAL_VARIABLE_TABLE,
                                           NULL,
                                           NULL,
                                           run_eval,
                                           GLOBAL_ENVIRONMENT,
                                           GLOBAL_MACRO_TABLE,
                                           GLOBAL_MODULE);
    
    return return_value;

}
/*
 * init walley language for javascript port
 */
void Walley_init_for_js(){
    // init walley language
    Walley_init();
    // run walley_core.wa (walley_core.h)
    Walley_RunString(WALLEY_CORE_CONTENT);

}
/*
 * run string and return string format of output
 */
char * Walley_RunStringAndReturnString(char * input_string){
    return to_string(Walley_RunString(input_string));
}

// compile to .wac file
void Walley_Compile(char * file_name){

    // get abs_path of file_name
    char abs_path[256];
    char working_path[256];
// get absolute path
#ifdef WIN32
	GetFullPathName((TCHAR*)file_name, 256, (TCHAR*)abs_path, NULL); // I don't know is this correct
#else
    if(!realpath(file_name, abs_path)){
        printf("ERROR: Failed to resolve absolute path of file %s\n", file_name);
        return;
    }
#endif
	strcpy(working_path, abs_path);
    // change working directory
    SwitchWorkingDirectory(working_path);
    
    COMPILATION_MODE = 1; // if under compilation mode, no print necessary
    GLOBAL_PUSH_SAVE_TO_VT = false; // GLOBAL_PUSH的时候不用把变量名字加入vt

    // read content from file
    FILE* file = fopen(abs_path, "r");
    if(file == NULL){
        printf("Failed to read file %s\n", abs_path);
        return; // fail to read
    }
    
    fseek(file, 0, SEEK_END);
    int64_t size = ftell(file);
    rewind(file);
    
    char* content = (char*)calloc(size + 1, 1);
    
    size_t result = fread(content,1,size,file);
    if (result != size) {
        printf("walley.h ERROR: Failed to read file %s\n", file_name);
        free(content);
        return;
    }
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
    
    // run walley_core.wa (walley_core.h)
    Walley_RunString(WALLEY_CORE_CONTENT);
    
    int32_t run_eval = true;
    
    //Environment * env = NULL;
    //int run_eval = false;
    
    //Object * v;
    
    p = lexer(content);
    if (p == NULL) {
        printf("ERROR: parentheses () num doesn't match");
    }
    o = parser(p);
    
    // compile
    /*v = */compiler_begin(insts,
                           o,
                           vt,
                           NULL,
                           NULL,
                           run_eval,
                           env,
                           mt,
                           GLOBAL_MODULE);
    
    free(content);
    
    //printf("INSTS LENGTH %llu\n", insts->length);
    //printf("CONSTANT TABLE LENGTH %llu\n", CONSTANT_TABLE_INSTRUCTIONS->length);
    
    char file_name_buffer[256];
    char inst_buffer[64];
    strcpy(file_name_buffer, abs_path);
    strcat(file_name_buffer, "c");
    file = fopen(file_name_buffer, "w");
    uint64_t i;
    
    // first 64 bits, length of CONSTANT_TABLE_INSTRUCTIONS
    sprintf(inst_buffer, "%04x ", (uint16_t)((CONSTANT_TABLE_INSTRUCTIONS->length & 0xFFFF000000000000) >> 48));
    fputs(inst_buffer, file);
    sprintf(inst_buffer, "%04x ", (uint16_t)((CONSTANT_TABLE_INSTRUCTIONS->length & 0x0000FFFF00000000) >> 32));
    fputs(inst_buffer, file);
    sprintf(inst_buffer, "%04x ", (uint16_t)((CONSTANT_TABLE_INSTRUCTIONS->length & 0x00000000FFFF0000) >> 16));
    fputs(inst_buffer, file);
    sprintf(inst_buffer, "%04x ", (uint16_t)((CONSTANT_TABLE_INSTRUCTIONS->length & 0x000000000000FFFF) >> 0));
    fputs(inst_buffer, file);

    // seconds 64 bits, length of INSTRUCTIONS
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

void quit_program_signal_handler(){
    printf("\n%s\n", "U pressed ctrl-c, program terminated");
    Walley_Finalize();
    exit(EXIT_SUCCESS);
}


#endif



