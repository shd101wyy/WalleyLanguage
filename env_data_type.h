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

static Object ** Constant_Pool; // used to save symbols and strings
static uint64_t Constant_Pool_Length;
static uint64_t Constant_Pool_Size;

static Object* CONSTANT_TABLE_FOR_COMPILATION;
static uint64_t   CONSTANT_TABLE_FOR_COMPILATION_LENGTH;

// instructions used to save contant...
static Instructions * CONSTANT_TABLE_INSTRUCTIONS;
static uint64_t   CONSTANT_TABLE_INSTRUCTIONS_TRACK_INDEX; // 保存上次的运行PC

static Instructions * GLOBAL_INSTRUCTIONS;
static Variable_Table * GLOBAL_VARIABLE_TABLE;
static Environment * GLOBAL_ENVIRONMENT;
static MacroTable * GLOBAL_MACRO_TABLE;

static Loaded_Modules * LOADED_MODULES;

static int8_t GLOBAL_PUSH_SAVE_TO_VT = 1;

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
    
    CLONE_STRING = Object_initString("clone", 5); // 16
    CLONE_STRING->use_count = 1;
    
    STRING_proto = Object_initString("proto", 5); // 17
    STRING_proto->use_count = 1;
    
    STRING_type = Object_initString("type", 4); // 18
    STRING_type->use_count = 1;
    
    STRING_object = Object_initString("object", 6); // 19
    STRING_object->use_count = 1;
    
    
    
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
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, CLONE_STRING, Object_initInteger(266));
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, STRING_proto, Object_initInteger(267));
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, STRING_type, Object_initInteger(268));
    Table_setval(CONSTANT_TABLE_FOR_COMPILATION, STRING_object, Object_initInteger(269));
    CONSTANT_TABLE_FOR_COMPILATION_LENGTH = 270; // set length
    
    // init Constant_Pool
    Constant_Pool = (Object**)malloc(sizeof(Object*)*1024);
    Constant_Pool_Size = 1024;
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
    Constant_Pool[266] = CLONE_STRING;
    Constant_Pool[267] = STRING_proto;
    Constant_Pool[268] = STRING_type;
    Constant_Pool[269] = STRING_object;
    
    Constant_Pool_Length = 270; // set length
    
    // init CONSTANT_TABLE_INSTRUCTIONS for compiler
    CONSTANT_TABLE_INSTRUCTIONS = Insts_init();
    CONSTANT_TABLE_INSTRUCTIONS_TRACK_INDEX = 0;
    
    // init global insts, vt, env, mt
    GLOBAL_INSTRUCTIONS = Insts_init();
    GLOBAL_VARIABLE_TABLE = VT_init();
    GLOBAL_ENVIRONMENT = createEnvironment();
    GLOBAL_MACRO_TABLE = MT_init();
    GLOBAL_MODULE = Module_init();
    
    // add variables offset to module
    for (i = 0; i < GLOBAL_VARIABLE_TABLE->frames[0]->length; i++) {
        Module_addOffset(GLOBAL_MODULE, i); // add offset
    }
    
    // init loaded_modules
	LOADED_MODULES = (Loaded_Modules*)malloc(sizeof(Loaded_Modules));
    LOADED_MODULES->next = NULL;
    LOADED_MODULES->file_abs_path = (char*)malloc(sizeof(char));
    LOADED_MODULES->file_abs_path[0] = 0;
    LOADED_MODULES->offset = 0;
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
	Environment_Frame * frame = (Environment_Frame*)malloc(sizeof(Environment_Frame));
    frame->length = 0;
    /* array 必须初始化为 0, 所以用calloc */
    frame->array = (Object**)calloc(size, sizeof(Object*));//malloc(sizeof(Object*)*size);
    frame->use_count = 0;
    return frame;
}
#define EF_set_builtin_lambda(v_, o_) ((v_)->array[(count)] = Object_initBuiltinLambda(o_)); count++;
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
	Environment * env = (Environment*)malloc(sizeof(Environment));
    env->frames = (Environment_Frame**)malloc(sizeof(Environment_Frame*) * size);
    env->length = 0;
    return env;
}

/*
 create frame0
 */
Environment_Frame *createFrame0(){
    // Object * frame = Object_initVector(0, GLOBAL_FRAME_SIZE);
	Environment_Frame * frame = (Environment_Frame*)malloc(sizeof(Environment_Frame));
    frame->length = 0;
    frame->array = GLOBAL_FRAME;
    frame->use_count = 0;
    
    uint32_t count = 0;
    
    // add builtin lambda                                // module:
    EF_set_builtin_lambda(frame, &builtin_cons);         // global
    EF_set_builtin_lambda(frame, &builtin_car);          // global
    EF_set_builtin_lambda(frame, &builtin_cdr);          // global
    EF_set_builtin_lambda(frame, &builtin_add);          // global
    EF_set_builtin_lambda(frame, &builtin_sub);          // global
    EF_set_builtin_lambda(frame, &builtin_mul);          // global
    EF_set_builtin_lambda(frame, &builtin_div);          // global
    EF_set_builtin_lambda(frame, &builtin_vector);       // global
    EF_set_builtin_lambda(frame, &builtin_vector_with_unchangable_length); // global
    EF_set_builtin_lambda(frame, &builtin_vector_length);// vector
    EF_set_builtin_lambda(frame, &builtin_vector_push);  // vector
    EF_set_builtin_lambda(frame, &builtin_vector_pop);   // vector
    EF_set_builtin_lambda(frame, &builtin_num_equal);    // global
    EF_set_builtin_lambda(frame, &builtin_num_lt);       // global
    EF_set_builtin_lambda(frame, &builtin_num_le);       // global
    EF_set_builtin_lambda(frame, &builtin_eq);           // global
    frame->array[count] = Object_initInteger(1); // eval // global
    frame->array[count]->use_count++;
    count++;
    EF_set_builtin_lambda(frame, &builtin_exit);         // global
    EF_set_builtin_lambda(frame, &builtin_gt);           // global
    EF_set_builtin_lambda(frame, &builtin_ge);           // global
    EF_set_builtin_lambda(frame, &builtin_parse);        // global
    EF_set_builtin_lambda(frame, &builtin_random);       // global
    EF_set_builtin_lambda(frame, &builtin_strcmp);       // global
    EF_set_builtin_lambda(frame, &builtin_string_slice); // string
    EF_set_builtin_lambda(frame, &builtin_string_length); // string
    EF_set_builtin_lambda(frame, &builtin_string_append); // string
    EF_set_builtin_lambda(frame, &builtin_make_table);    // global
    EF_set_builtin_lambda(frame, &builtin_table_keys);    // table
    EF_set_builtin_lambda(frame, &builtin_table_delete);  // table
    EF_set_builtin_lambda(frame, &builtin_file_read);     // file
    EF_set_builtin_lambda(frame, &builtin_file_write);    // file
    // sys-argv
    frame->array[count] = SYS_ARGV;                       // sys
    frame->array[count]->use_count++;
    count++;
    EF_set_builtin_lambda(frame, &builtin_int_to_string);    // int
    EF_set_builtin_lambda(frame, &builtin_float_to_string);  // float
    EF_set_builtin_lambda(frame, &builtin_input);            // global
    EF_set_builtin_lambda(frame, &builtin_display_string);   // global
    EF_set_builtin_lambda(frame, &builtin_string_to_int);    // string
    EF_set_builtin_lambda(frame, &builtin_string_to_float);  // string
    
    EF_set_builtin_lambda(frame, &builtin_null_type);       // global
    EF_set_builtin_lambda(frame, &builtin_numer);            // ratio
    EF_set_builtin_lambda(frame, &builtin_denom);            // ratio
    EF_set_builtin_lambda(frame, &builtin_gensym);           // global
    EF_set_builtin_lambda(frame, &builtin_table_add_proto);    // global
    EF_set_builtin_lambda(frame, &builtin_table_proto);        // global
    EF_set_builtin_lambda(frame, &builtin_typeof);           // global
    EF_set_builtin_lambda(frame, &builtin_cos);              // math
    EF_set_builtin_lambda(frame, &builtin_sin);              // math
    EF_set_builtin_lambda(frame, &builtin_tan);              // math
    EF_set_builtin_lambda(frame, &builtin_acos);             // math
    EF_set_builtin_lambda(frame, &builtin_asin);             // math
    EF_set_builtin_lambda(frame, &builtin_atan);             // math
    EF_set_builtin_lambda(frame, &builtin_cosh);             // math
    EF_set_builtin_lambda(frame, &builtin_sinh);             // math
    EF_set_builtin_lambda(frame, &builtin_tanh);             // math
    EF_set_builtin_lambda(frame, &builtin_log);              // math
    EF_set_builtin_lambda(frame, &builtin_exp);              // math
    EF_set_builtin_lambda(frame, &builtin_log10);            // math
    EF_set_builtin_lambda(frame, &builtin_pow);              // math
    EF_set_builtin_lambda(frame, &builtin_sqrt);             // math
    EF_set_builtin_lambda(frame, &builtin_ceil);             // math
    EF_set_builtin_lambda(frame, &builtin_floor);            // math
    EF_set_builtin_lambda(frame, &builtin_string_find);      // string
    EF_set_builtin_lambda(frame, &builtin_string_replace);   // string
    frame->array[count] = Object_initInteger(2); // apply    // global
    frame->array[count]->use_count++;
    count++;
    EF_set_builtin_lambda(frame, &builtin_vector_slice);      // vector
    EF_set_builtin_lambda(frame, &builtin_set_car);           // global
    EF_set_builtin_lambda(frame, &builtin_set_cdr);           // global
    EF_set_builtin_lambda(frame, &builtin_system);            // global
    EF_set_builtin_lambda(frame, &builtin_left_shift);        // global
    EF_set_builtin_lambda(frame, &builtin_right_shift);       // global
    EF_set_builtin_lambda(frame, &builtin_unsigned_right_shift); // global
    EF_set_builtin_lambda(frame, &builtin_bitwise_and);       // global
    EF_set_builtin_lambda(frame, &builtin_bitwise_or);        // global
    EF_set_builtin_lambda(frame, &builtin_abs_path);          // file
    EF_set_builtin_lambda(frame, &builtin_float_to_int64);          // float
    EF_set_builtin_lambda(frame, &builtin_table_length); // table
    EF_set_builtin_lambda(frame, &builtin_string_char_code_at); // string
    EF_set_builtin_lambda(frame, &builtin_string_from_char_code); // string
    EF_set_builtin_lambda(frame, &builtin_os_fork); // os
    EF_set_builtin_lambda(frame, &builtin_os_getpid); // os
    EF_set_builtin_lambda(frame, &builtin_os_getppid); // os
    EF_set_builtin_lambda(frame, &builtin_os_waitpid); // os
    EF_set_builtin_lambda(frame, &builtin_os_getenv); // os
    EF_set_builtin_lambda(frame, &builtin_sleep); // global
    EF_set_builtin_lambda(frame, &builtin_modulo); // global
    
    frame->length = count; // set length
    return frame;
}
/*
 create environment
 */
Environment *createEnvironment(){
	Environment * env = (Environment*)malloc(sizeof(Environment));
    env->length = 1;
	env->frames = (Environment_Frame**)malloc(sizeof(Environment_Frame*) * MAX_STACK_SIZE);
    env->frames[0] = createFrame0();
    env->frames[0]->use_count = 1;
    return env;
}
/*
 copy environment
 */
Environment *copyEnvironment(Environment * old_env){
	Environment * new_env = (Environment*)malloc(sizeof(Environment));
    new_env->length = old_env->length;
	new_env->frames = (Environment_Frame**)malloc(sizeof(Environment_Frame*) * new_env->length);
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
	Environment * new_env = (Environment*)malloc(sizeof(Environment));
    new_env->length = old_env->length;
	new_env->frames = (Environment_Frame**)malloc(sizeof(Environment_Frame*) * (new_env->length + 1));
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
