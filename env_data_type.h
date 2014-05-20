//
//  env_data_type.h
//  walley
//
//  Created by WangYiyi on 4/29/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_env_data_type_h
#define walley_env_data_type_h
#include "compiler_data_type.h"

#define MAX_STACK_SIZE 1024
#define GLOBAL_FRAME_SIZE 1024
static Object * GLOBAL_FRAME[GLOBAL_FRAME_SIZE];

static Object * Constant_Pool[1024]; // used to save symbols and strings
static uint32_t Constant_Pool_Length;

static Object* CONSTANT_TABLE_FOR_COMPILATION;
static uint64_t   CONSTANT_TABLE_FOR_COMPILATION_LENGTH;

// instructions used to save contant...
static Instructions * CONSTANT_TABLE_INSTRUCTIONS;
static uint64_t   CONSTANT_TABLE_INSTRUCTIONS_TRACK_INDEX; // 保存上次的运行PC

static Instructions * GLOBAL_INSTRUCTIONS;
static Variable_Table * GLOBAL_VARIABLE_TABLE;
static Environment * GLOBAL_ENVIRONMENT;
static MacroTable * GLOBAL_MACRO_TABLE;

Environment *createEnvironment(); // init env

/*
 Init Walley Languge
 Set necessary values
 */
void Walley_init(){
    // init several constants
    GLOBAL_NULL = Object_initNull(); // init GLOBAL_NULL
    GLOBAL_NULL->use_count = 1;
    
    QUOTE_STRING = Object_initString("quote", 5); // 0
    QUOTE_STRING->use_count = 1;
    
    UNQUOTE_STRING = Object_initString("unquote", 7); // 1
    UNQUOTE_STRING->use_count = 1;
    
    UNQUOTE_SPLICE_STRING = Object_initString("unquote-splice", 14); // 2
    UNQUOTE_SPLICE_STRING->use_count = 1;
    
    QUASIQUOTE_STRING = Object_initString("quasiquote", 10); // 3
    QUASIQUOTE_STRING->use_count = 1;
    
    CONS_STRING = Object_initString("cons", 4); // 4
    CONS_STRING->use_count = 1;
    
    DEF_STRING = Object_initString("def", 3); // 5
    DEF_STRING->use_count = 1;
    
    SET_STRING = Object_initString("set!", 4); // 6
    SET_STRING->use_count = 1;
    
    LAMBDA_STRING = Object_initString("lambda", 6); // 7
    LAMBDA_STRING->use_count = 1;
    
    GLOBAL_TRUE = Object_initString("true", 4); // 8
    GLOBAL_TRUE->use_count = 1;
    
    INTEGER_STRING = Object_initString("integer", 7);// 9
    INTEGER_STRING->use_count = 1;
 
    FLOAT_STRING = Object_initString("float", 5); // 10
    FLOAT_STRING->use_count = 1;
    
    RATIO_STRING = Object_initString("ratio", 5); // 11
    RATIO_STRING->use_count = 1;
    
    STRING_STRING = Object_initString("string", 6); // 12
    STRING_STRING->use_count = 1;
    
    PAIR_STRING = Object_initString("pair", 4); // 13
    PAIR_STRING->use_count = 1;
    
    VECTOR_STRING = Object_initString("vector", 6); // 14
    VECTOR_STRING->use_count = 1;
    
    TABLE_STRING = Object_initString("table", 5); // 15
    TABLE_STRING->use_count = 1;
    
    
    
    CONSTANT_TABLE_FOR_COMPILATION = Object_initTable(1024); // init constant table
    // add those constants to table
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, QUOTE_STRING, Object_initInteger(250));         // 0
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, UNQUOTE_STRING, Object_initInteger(251));       // 1
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, UNQUOTE_SPLICE_STRING, Object_initInteger(252));// 2
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, QUASIQUOTE_STRING, Object_initInteger(253));    // 3
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, CONS_STRING, Object_initInteger(254));          // 4
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, DEF_STRING, Object_initInteger(255));           // 5
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, SET_STRING, Object_initInteger(256));        // 6
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, LAMBDA_STRING, Object_initInteger(257));          // 7
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, GLOBAL_TRUE, Object_initInteger(258));         // 8
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, INTEGER_STRING, Object_initInteger(259));         // 9
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, FLOAT_STRING, Object_initInteger(260));
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, RATIO_STRING, Object_initInteger(261));
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, STRING_STRING, Object_initInteger(262));
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, PAIR_STRING, Object_initInteger(263));
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, VECTOR_STRING, Object_initInteger(264));
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, TABLE_STRING, Object_initInteger(265));
    
    CONSTANT_TABLE_FOR_COMPILATION_LENGTH = 266; // set length
    
    // create constant integer pool
    // 0 ~ 249
    int32_t i;
    for(i = 0; i < 250; i++){
        Object * t = Object_initInteger(i);
        t->use_count = 1;
        Constant_Pool[i] = t;
    }
    
    
    // init Constant Pool according to CONSTANT_TABLE_FOR_COMPILATION
    Constant_Pool[250] = QUOTE_STRING;
    Constant_Pool[251] = UNQUOTE_STRING;
    Constant_Pool[252] = UNQUOTE_SPLICE_STRING;
    Constant_Pool[253] = QUASIQUOTE_STRING;
    Constant_Pool[254] = CONS_STRING;
    Constant_Pool[255] = DEF_STRING;
    Constant_Pool[256] = SET_STRING;
    Constant_Pool[257] = LAMBDA_STRING;
    Constant_Pool[258] = GLOBAL_TRUE;
    Constant_Pool[259] = INTEGER_STRING;
    Constant_Pool[260] = FLOAT_STRING;
    Constant_Pool[261] = RATIO_STRING;
    Constant_Pool[262] = STRING_STRING;
    Constant_Pool[263] = PAIR_STRING;
    Constant_Pool[264] = VECTOR_STRING;
    Constant_Pool[265] = TABLE_STRING;
    
    Constant_Pool_Length = 266; // set length
    
    // init CONSTANT_TABLE_INSTRUCTIONS for compiler
    CONSTANT_TABLE_INSTRUCTIONS = Insts_init();
    CONSTANT_TABLE_INSTRUCTIONS_TRACK_INDEX = 0;
    
    // init global insts, vt, env, mt
    GLOBAL_INSTRUCTIONS = Insts_init();
    GLOBAL_VARIABLE_TABLE = VT_init();
    GLOBAL_ENVIRONMENT = createEnvironment();
    GLOBAL_MACRO_TABLE = MT_init();
}

// end walley program
void Walley_Finalize(){
    Object_free(CONSTANT_TABLE_FOR_COMPILATION);
    int32_t i;
    for (i = 0; i < Constant_Pool_Length; i++) {
        Constant_Pool[i]->use_count--;
        Object_free(Constant_Pool[i]);
    }
    
    free(CONSTANT_TABLE_INSTRUCTIONS->array);
    free(CONSTANT_TABLE_INSTRUCTIONS);
    
    free(GLOBAL_INSTRUCTIONS->array);
    free(GLOBAL_INSTRUCTIONS);
    
    VT_free(GLOBAL_VARIABLE_TABLE);
    
    Env_free(GLOBAL_ENVIRONMENT);
    
    for (i = 0; i < GLOBAL_MACRO_TABLE->length; i++) {
        int32_t j;
        MacroTableFrame * mtf = GLOBAL_MACRO_TABLE->frames[i];
        for (j = 0; j < mtf->length; j++) {
            Macro_free(mtf->array[j]);
        }
        free(mtf->array);
        free(mtf);
    }
    // free(GLOBAL_MACRO_TABLE->frames);
    free(GLOBAL_MACRO_TABLE);
    
}

/*
 construct frame for environment
 */
struct Environment_Frame {
    Object ** array;
    int32_t length;
    int32_t use_count;
};

/*
 free frame
 */
void EF_free(Environment_Frame * ef){
    if (ef->use_count == 0) {
        int32_t i;
        for (i = 0; i < ef->length; i++) {
            ef->array[i]->use_count--;
            Object_free(ef->array[i]);
        }
        free(ef->array);
        free(ef);
    }
    return;
}
/*
 create frame with size
 */
Environment_Frame * EF_init_with_size(int32_t size){
    Environment_Frame * frame = malloc(sizeof(Environment_Frame));
    frame->length = 0;
    frame->array = malloc(sizeof(Object*)*size);
    frame->use_count = 0;
    return frame;
}
#define EF_set_builtin_lambda(v_, index, o_) ((v_)->array[(index)] = Object_initBuiltinLambda(o_))
/*
 construct environment
 */
struct Environment{
    Environment_Frame ** frames;
    int32_t length;  // max length MAX_STACK_SIZE
};

void Env_free(Environment * env){
    int32_t i;
    for (i = 0; i < env->length; i++) {
        env->frames[i]->use_count--;
        EF_free(env->frames[i]);
    }
    free(env->frames);
    free(env);
    return;
}
Environment * Env_init_with_size(int32_t size){
    Environment * env = malloc(sizeof(Environment));
    env->frames = (Environment_Frame**)malloc(sizeof(Environment_Frame*) * size);
    env->length = 0;
    return env;
}

/*
 create frame0
 */
Environment_Frame *createFrame0(){
    // Object * frame = Object_initVector(0, GLOBAL_FRAME_SIZE);
    Environment_Frame * frame = malloc(sizeof(Environment_Frame));
    frame->length = 0;
    frame->array = GLOBAL_FRAME;
    frame->use_count = 0;
    
    // add builtin lambda
    EF_set_builtin_lambda(frame, 0, &builtin_cons);
    EF_set_builtin_lambda(frame, 1, &builtin_car);
    EF_set_builtin_lambda(frame, 2, &builtin_cdr);
    EF_set_builtin_lambda(frame, 3, &builtin_add);
    EF_set_builtin_lambda(frame, 4, &builtin_sub);
    EF_set_builtin_lambda(frame, 5, &builtin_mul);
    EF_set_builtin_lambda(frame, 6, &builtin_div);
    EF_set_builtin_lambda(frame, 7, &builtin_vector);
    EF_set_builtin_lambda(frame, 8, &builtin_vector_with_unchangable_length);
    EF_set_builtin_lambda(frame, 9, &builtin_vector_length);
    EF_set_builtin_lambda(frame, 10, &builtin_vector_push);
    EF_set_builtin_lambda(frame, 11, &builtin_vector_pop);
    EF_set_builtin_lambda(frame, 12, &builtin_num_equal);
    EF_set_builtin_lambda(frame, 13, &builtin_num_lt);
    EF_set_builtin_lambda(frame, 14, &builtin_num_le);
    EF_set_builtin_lambda(frame, 15, &builtin_eq);
    frame->array[16] = Object_initInteger(1); // eval
    frame->array[16]->use_count++;
    EF_set_builtin_lambda(frame, 17, &builtin_exit);
    EF_set_builtin_lambda(frame, 18, &builtin_gt);
    EF_set_builtin_lambda(frame, 19, &builtin_ge);
    EF_set_builtin_lambda(frame, 20, &builtin_parse);
    EF_set_builtin_lambda(frame, 21, &builtin_random);
    EF_set_builtin_lambda(frame, 22, &builtin_strcmp);
    EF_set_builtin_lambda(frame, 23, &builtin_string_slice);
    EF_set_builtin_lambda(frame, 24, &builtin_string_length);
    EF_set_builtin_lambda(frame, 25, &builtin_string_append);
    EF_set_builtin_lambda(frame, 26, &builtin_make_table);
    EF_set_builtin_lambda(frame, 27, &builtin_table_keys);
    EF_set_builtin_lambda(frame, 28, &builtin_table_delete);
    EF_set_builtin_lambda(frame, 29, &builtin_file_read);
    EF_set_builtin_lambda(frame, 30, &builtin_file_write);
    // sys-argv
    frame->array[31] = SYS_ARGV;
    EF_set_builtin_lambda(frame, 32, &builtin_int_to_string);
    EF_set_builtin_lambda(frame, 33, &builtin_float_to_string);
    EF_set_builtin_lambda(frame, 34, &builtin_input);
    EF_set_builtin_lambda(frame, 35, &builtin_display_string);
    EF_set_builtin_lambda(frame, 36, &builtin_string_to_int);
    EF_set_builtin_lambda(frame, 37, &builtin_string_to_float);
    
    EF_set_builtin_lambda(frame, 38, &builtin_ratio_type);
    EF_set_builtin_lambda(frame, 39, &builtin_numer);
    EF_set_builtin_lambda(frame, 40, &builtin_denom);
    EF_set_builtin_lambda(frame, 41, &builtin_gensym);
    EF_set_builtin_lambda(frame, 42, &builtin_table_add_tag);
    EF_set_builtin_lambda(frame, 43, &builtin_table_tag);
    EF_set_builtin_lambda(frame, 44, &builtin_typeof);
    EF_set_builtin_lambda(frame, 45, &builtin_cos);
    EF_set_builtin_lambda(frame, 46, &builtin_sin);
    EF_set_builtin_lambda(frame, 47, &builtin_tan);
    EF_set_builtin_lambda(frame, 48, &builtin_acos);
    EF_set_builtin_lambda(frame, 49, &builtin_asin);
    EF_set_builtin_lambda(frame, 50, &builtin_atan);
    EF_set_builtin_lambda(frame, 51, &builtin_cosh);
    EF_set_builtin_lambda(frame, 52, &builtin_sinh);
    EF_set_builtin_lambda(frame, 53, &builtin_tanh);
    EF_set_builtin_lambda(frame, 54, &builtin_log);
    EF_set_builtin_lambda(frame, 55, &builtin_exp);
    EF_set_builtin_lambda(frame, 56, &builtin_log10);
    EF_set_builtin_lambda(frame, 57, &builtin_pow);
    EF_set_builtin_lambda(frame, 58, &builtin_sqrt);
    EF_set_builtin_lambda(frame, 59, &builtin_ceil);
    EF_set_builtin_lambda(frame, 60, &builtin_floor);
    EF_set_builtin_lambda(frame, 61, &builtin_string_find);
    EF_set_builtin_lambda(frame, 62, &builtin_string_replace);
    frame->array[63] = Object_initInteger(2); // apply
    frame->array[63]->use_count++;
    EF_set_builtin_lambda(frame, 64, builtin_vector_slice);
    EF_set_builtin_lambda(frame, 65, builtin_set_car);
    EF_set_builtin_lambda(frame, 66, builtin_set_cdr);
    frame->length = 67; // set length
    return frame;
}
/*
 create environment
 */
Environment *createEnvironment(){
    Environment * env = malloc(sizeof(Environment));
    env->length = 1;
    env->frames = malloc(sizeof(Environment_Frame*) * MAX_STACK_SIZE);
    env->frames[0] = createFrame0();
    env->frames[0]->use_count = 1;
    return env;
}
/*
 copy environment
 */
Environment *copyEnvironment(Environment * old_env){
    Environment * new_env = malloc(sizeof(Environment));
    new_env->length = old_env->length;
    new_env->frames = malloc(sizeof(Environment_Frame*) * new_env->length);
    int32_t i;
    for (i = 0; i < new_env->length; i++) {
        new_env->frames[i] = old_env->frames[i]; // copy frame pointer
        new_env->frames[i]->use_count++; // increase use count
    }
    return new_env;
}

/*
 copy environment and push frame
 */
Environment *copyEnvironmentAndPushFrame(Environment * old_env, Environment_Frame * frame){
    Environment * new_env = malloc(sizeof(Environment));
    new_env->length = old_env->length;
    new_env->frames = malloc(sizeof(Environment_Frame*) * (new_env->length + 1));
    int32_t i;
    for (i = 0; i < old_env->length; i++) {
        new_env->frames[i] = old_env->frames[i]; // copy frame pointer
        new_env->frames[i]->use_count++; // increase use count
    }
    new_env->frames[i] = frame;
    frame->use_count++;
    new_env->length+=1;
    return new_env;
}
#endif
