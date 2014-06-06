//
//  compiler_data_type.h
//  walley
//
//  Created by WangYiyi on 4/27/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//
#ifndef walley_compiler_data_type_h
#define walley_compiler_data_type_h
#include "parser.h"

/* opcodes */
#define SET 0x0
#define GET 0x1
#define CONST 0x2

#define CONST  0x2
#define CONST_INTEGER  0x2100
#define CONST_FLOAT 0x2200
#define CONST_STRING 0x2300
#define CONST_NULL 0x2400
#define CONST_LOAD 0x2500

#define MAKELAMBDA 0x3
#define RETURN 0x4
#define NEWFRAME 0x5
#define PUSH_ARG 0x6
#define CALL 0x7
#define JMP 0x8
#define TEST 0x9
#define SET_TOP 0xA
#define TAIL_CALL_PUSH 0xB


static Module * GLOBAL_MODULE;

/*
    construct Instructions data type
 */
typedef struct Instructions{
    uint16_t * array;
    uint64_t length;
    uint64_t size;
    uint64_t start_pc;
}Instructions;
/*
    init insts
 */
Instructions * Insts_init(){
    Instructions * insts = (Instructions*)malloc(sizeof(Instructions));
    insts->length = 0;
    insts->size = 1024;
    insts->array = (uint16_t*)malloc(sizeof(uint16_t)*(insts->size));
    insts->start_pc = 0;
    return insts;
}
/*
    Instructions push
 */
void Insts_push(Instructions * insts, uint16_t v){
    if(insts->length == insts->size){ // reach maximum
        insts->size*=2;
        insts->array = (uint16_t*)realloc(insts->array, sizeof(uint16_t)*insts->size);
    }
    insts->array[insts->length] = v;
    insts->length++;
}
/*
    print insts
 */
void printInstructions(Instructions * insts){
    uint64_t length = insts->length;
    uint64_t i;
    for (i = 0; i < length; i++) {
        printf("%04x ", insts->array[i]);
    }
    return;
}
#define Insts_length(l) ((l)->length)
#define Insts_get(l, index) ((l)->array[(index)])
#define Insts_set(l, index, v) ((l)->array[(index)] = (v))
/*
    define Variable Table Frame
*/
typedef struct Variable_Table_Frame{
    char ** var_names;
    uint32_t length;
    uint32_t use_count;
} Variable_Table_Frame;

/*
    init vtf
 */
Variable_Table_Frame * VTF_init(uint32_t size){
    Variable_Table_Frame * vtf = malloc(sizeof(Variable_Table_Frame));
    vtf->var_names = (char**)malloc(sizeof(char*)*size);
    vtf->length = 0;
    vtf->use_count = 0;
    return vtf;
}
/* 
    push var_name to vtf
 */
void VTF_push(Variable_Table_Frame * vtf, char * value){
    if (value == NULL) {
        vtf->var_names[vtf->length] = NULL;
        vtf->length++;
        return;
    }
    // printf("VTF_push %s %ld\n", value, strlen(value));
    char * s = (char*)malloc(sizeof(char)* (strlen(value)+ 1));
    strcpy(s, value);
    vtf->var_names[vtf->length] = s;
    vtf->length++;
}

/*
    free variabel table frame
 */
void VTF_free(Variable_Table_Frame * vtf){
    uint32_t length = vtf->length;
    uint32_t i ;
    for (i = 0; i < length; i++) {
        free(vtf->var_names[i]);
    }
    free(vtf);
    vtf = NULL;
    return;
}


/*
    define Variable Table
 */
#define VARIABLE_TABLE_MAX_SIZE 64
#define FRAME0_SIZE 1024
#define MAX_VAR_NAME_LENGTH 32
/* Variable Table Data Structure */
typedef struct Variable_Table{
    Variable_Table_Frame * frames[VARIABLE_TABLE_MAX_SIZE];
    uint32_t length;
    // unsigned int use_count;
} Variable_Table;

// push value to VT's #frame_index frame
#define VT_push(vt, frame_index, value) VTF_push((vt)->frames[(frame_index)], (value))

/*
  init Variable Table
 */
Variable_Table * VT_init(){
    Variable_Table * vt = malloc(sizeof(Variable_Table));
    vt->frames[0] = VTF_init(FRAME0_SIZE);
    vt->frames[0]->use_count = 1; // in use
    
    vt->length = 1; // 有一个frame
    //vt->use_count = 1; // in use

    // set frame0                           // module
    VT_push(vt, 0, "cons");                 // global 0
    VT_push(vt, 0, "car");                  // global 1
    VT_push(vt, 0, "cdr");                  // global 2
    VT_push(vt, 0, "+");                    // global 3
    VT_push(vt, 0, "-");                    // global 4
    VT_push(vt, 0, "*");                    // global 5
    VT_push(vt, 0, "/");                    // global 6
    VT_push(vt, 0, "vector!");              // global 7
    VT_push(vt, 0, "vector");               // global 8
    VT_push(vt, 0, "length");               // vector 9
    
    VT_push(vt, 0, "push!");                // vector 10
    VT_push(vt, 0, "pop!");                 // vector 11
    VT_push(vt, 0, "=");                    // global 12
    VT_push(vt, 0, "<");                    // global 13
    VT_push(vt, 0, "<=");                   // global 14
    VT_push(vt, 0, "eq?");                  // global 15
    VT_push(vt, 0, "eval");  // 16          // global 16
    VT_push(vt, 0, "exit");  // 17          // global 17
    VT_push(vt, 0, ">");// 18               // global 18
    VT_push(vt, 0, ">="); // 19             // global 19
    
    VT_push(vt, 0, "parse"); // 20          // global 20
    VT_push(vt, 0, "random"); // 21         // global 21
    VT_push(vt, 0, "strcmp");               // global 22
    VT_push(vt, 0, "slice");                // string 23
    VT_push(vt, 0, "length");               // string 24
    VT_push(vt, 0, "append");               // string 25
    VT_push(vt, 0, "table");                // global 26
    VT_push(vt, 0, "keys");                 // table  27
    VT_push(vt, 0, "delete");               // table  28
    VT_push(vt, 0, "read");                 // file   29
    
    VT_push(vt, 0, "write");                // file 30
    VT_push(vt, 0, "argv");                 // sys  31
    VT_push(vt, 0, "->string");             // int  32
    VT_push(vt, 0, "->string");             // float  33
    VT_push(vt, 0, "input");                // global 34
    VT_push(vt, 0, "print");                // global 35
    VT_push(vt, 0, "->int");                // string 36
    VT_push(vt, 0, "->float");              // string 37
    VT_push(vt, 0, "ratio?");               // global 38
    VT_push(vt, 0, "numer");                // ratio  39

    VT_push(vt, 0, "denom");                // ratio  40
    VT_push(vt, 0, "gensym");               // global 41
    VT_push(vt, 0, "add-tag");              // table  42
    VT_push(vt, 0, "tag");                  // table  43
    VT_push(vt, 0, "typeof");  // 44        // global 44
    VT_push(vt, 0, "cos");  // 45           // math   45
    VT_push(vt, 0, "sin");                  // math   46
    VT_push(vt, 0, "tan");                  // math   47
    VT_push(vt, 0, "acos");                 // math   48
    VT_push(vt, 0, "asin");                 // math   49
    
    VT_push(vt, 0, "atan");  // 50          // math   50
    VT_push(vt, 0, "cosh");  // 51          // math   51
    VT_push(vt, 0, "sinh");  // 52          // math   52
    VT_push(vt, 0, "tanh");  // 53          // math   53
    VT_push(vt, 0, "log");  // 54           // math   54
    VT_push(vt, 0, "exp");  // 55           // math   55
    VT_push(vt, 0, "log10");  // 56         // math   56
    VT_push(vt, 0, "pow");  // 57           // math   57
    VT_push(vt, 0, "sqrt");  // 58          // math   58
    VT_push(vt, 0, "ceil");  // 59          // math   59

    VT_push(vt, 0, "floor");  // 60         // math   60
    VT_push(vt, 0, "find");  // 61          // string 61
    VT_push(vt, 0, "replace");  // 62       // string 62
    VT_push(vt, 0, "apply");   // 63        // global 63
    VT_push(vt, 0, "slice"); // 64          // vector 64
    VT_push(vt, 0, "set-car!"); // 65       // global 65
    VT_push(vt, 0, "set-cdr!"); // 66       // global 66
    VT_push(vt, 0, "cmd"); // 67            // sys 67
    return vt;
}

void Variable_Table_push_frame(Variable_Table * vt, Variable_Table_Frame * vtf){
    vtf->use_count++; // increase use_count of vtf
    
    vt->frames[vt->length] = vtf;
    vt->length++;
}

/* append empty frame */
void VT_add_new_empty_frame(Variable_Table * vt){
    Variable_Table_Frame * frame = VTF_init(64);
    frame->use_count = 1; // increase use_count of vtf
    vt->frames[vt->length] = frame;
    vt->length+=1;
}
/*
    克隆 Variable Table
 */
Variable_Table * VT_copy(Variable_Table * vt){
    Variable_Table * return_vt;
    uint32_t length = vt->length;
    uint32_t i;
    return_vt = malloc(sizeof(Variable_Table));
    return_vt->length = length;
    for (i = 0; i < length; i++) {
        return_vt->frames[i] = vt->frames[i]; // 没有copy frame deeply
        vt->frames[i]->use_count++; // in use ++
    }
    return return_vt;
}

/*
    free Variable Table
 */
void VT_free(Variable_Table * vt){
    uint32_t length = vt->length;
    uint32_t i = 0;
    for (i = 0; i < length; i++) {
        //vt->frames[i]->use_count--;
        //if (vt->frames[i]->use_count == 0) { // 只有在没人使用的时候free
            VTF_free(vt->frames[i]);
            vt->frames[i] = NULL;
        //}
    }
    free(vt->frames);
    //free(vt);
    return;
}


/*
 this saved lambda is for tail call optimization and compilation
 */
typedef struct Lambda_for_Compilation{
    uint32_t param_num;
    uint32_t variadic_place;
    uint64_t start_pc;
    Variable_Table * vt;
    int32_t is_tail_call : 1;
}Lambda_for_Compilation;

void LFC_free(Lambda_for_Compilation * func){
    if(func->vt != NULL)
        VT_free(func->vt);
}

/*
    Macro Data Type
 */

typedef struct Macro{
    char * macro_name;
    Object * clauses;
    Variable_Table * vt;
}Macro;

Macro * Macro_init(char * macro_name, Object * clauses, Variable_Table * vt){
    Macro * m = malloc(sizeof(Macro));
    char * s = malloc(sizeof(char)*(strlen(macro_name)+1));
    strcpy(s, macro_name);

    m->macro_name = s;
    m->clauses = clauses;
    m->vt = vt;
    
    clauses->use_count++;

    return m;
}
/*
Object * Macro_copy_clauses(Object * clauses){
    if (clauses == GLOBAL_NULL) {
        return GLOBAL_NULL;
    }
    Object * v = car(clauses);
    switch (v->type) {
        case NULL_:
            return cons(GLOBAL_NULL,
                        Macro_copy_clauses(cdr(clauses)));
        case INTEGER:
            return cons(Object_initInteger(v->data.Integer.v), Macro_copy_clauses(cdr(clauses)));
        case DOUBLE:
            return cons(Object_initDouble(v->data.Double.v), Macro_copy_clauses(cdr(clauses)));
        case PAIR:
            return cons(Macro_copy_clauses(v),
                        Macro_copy_clauses(cdr(clauses)));
        case STRING:
            return cons(Object_initString(v->data.String.v, v->data.String.length), Macro_copy_clauses(cdr(clauses)));
        default:
            printf("### ERROR please report bug: Macro_copy_clauses");
            return GLOBAL_NULL;
    }
}*/

void Macro_free(Macro * macro){
    // free macro name
    free(macro->macro_name);
    
    // free clauses
    macro->clauses->use_count--;
    Object_free(macro->clauses);
    
    // only need to free top frame
    Variable_Table_Frame * top_frame = macro->vt->frames[macro->vt->length - 1];
    int32_t length = top_frame->length;
    int32_t i = 0;
    for (i = 0; i < length; i++) {
        free(top_frame->var_names[i]);
    }
    free(top_frame->var_names);
    free(top_frame);
    free(macro->vt);
    
    free(macro);
}
/*
    Macro Table Frame
 */
typedef struct MacroTableFrame{
    Macro ** array;
    uint32_t size;
    uint32_t length;
}MacroTableFrame;

MacroTableFrame * MTF_init(int32_t size){
    MacroTableFrame * o = malloc(sizeof(MacroTableFrame));
    o->size = size;
    o->length = 0;
    o->array = malloc(sizeof(Macro*) * (o->size));
    return o;
}
/*
    Macro Table
 */
typedef struct MacroTable {
    MacroTableFrame * frames[VARIABLE_TABLE_MAX_SIZE];
    uint32_t length;
}MacroTable;

MacroTable * MT_init(){
    MacroTable * o = malloc(sizeof(MacroTable));
    o->length = 1; // only have one frame
    o->frames[0] = MTF_init(64);
    return o;
}
void MT_find(MacroTable * mt, char * macro_name, int32_t output[2]){
    uint32_t length = mt->length;
    uint32_t frame_length;
    uint32_t i, j;
    for (i = 0; i < length; i++) {
        MacroTableFrame * frame = mt->frames[i];
        frame_length = frame->length;
        for (j = 0; j < frame_length; j++) {
            if (str_eq(frame->array[j]->macro_name, macro_name)) {
                output[0] = i;
                output[1] = j;
                return;
            }
        }
    }
    output[0] = -1;
    output[1] = -1;
    return;
}

/* append empty frame */
void MT_add_new_empty_frame(MacroTable * mt){
    MacroTableFrame * frame = MTF_init(64);
    //frame->use_count = 1; // increase use_count of vtf
    mt->frames[mt->length] = frame;
    mt->length+=1;
}
/*
 克隆 Variable Table
 */
MacroTable * MT_copy(MacroTable * mt){
    MacroTable * return_mt;
    uint32_t length = mt->length;
    uint32_t i;
    return_mt = malloc(sizeof(MacroTable));
    return_mt->length = length;
    for (i = 0; i < length; i++) {
        return_mt->frames[i] = mt->frames[i]; // 没有copy frame deeply
    }
    return return_mt;
}


/*
 *   construct Module data structure
 *   定义模块结构
 *
 */
struct Module{
    char * module_as_name;              // 但前所在的module的 as_name
    Module * children_modules_list;     // 当前module中load的所有modules
    Module * next;                      // 下一个 module
    /*
     *  例如 (def x 2) (def y 3)
     *  vtf_offset [1, 2]   length 2
     *  例如 (def x 2) (def y 3) (load blabla后vtf断了) (def z 4)
     *  vtf_offset [1, 2, 12]  length 3
     *  找变量名的时候根据 vtf_offset 从 GLOBAL Variable Frame 中找
     */
    uint16_t * vtf_offset;          // variable table frame的开始offset, 最多存256个？
    uint16_t length;                   // length of vtf_offset
    uint16_t size;
};

/*
 *
 *  initialize Module data structure
 *
 */
Module * Module_init(char * module_as_name){
    Module * m = malloc(sizeof(Module));
    if (module_as_name == NULL) {
        m->module_as_name = NULL;
    }
    else{
        m->module_as_name = malloc(sizeof(char) * (strlen(module_as_name) + 1));
        strcpy(m->module_as_name, module_as_name);
    }
    m->children_modules_list = NULL;
    m->next = NULL;
    m->size = 8;
    m->length = 0;
    m->vtf_offset = malloc(sizeof(uint16_t) * m->size);
    
    return m;
}

/*
 * init global module
 */
Module * Module_initializeGlobalModule(){
    Module * m = Module_init("global");
    /*
     *  todo : register string module, file module ...
     *
     */
    
    return m;
}

/*
 * push offset
 */
void Module_pushVarOffset(Module * m, uint16_t offset){
    if (m->size == m->length) {
        m->size *= 2;
        m->vtf_offset = (uint16_t*)realloc(m->vtf_offset, sizeof(uint16_t)*m->size);
    }
    m->vtf_offset[m->length] = offset;
    m->length++;
}
/*
 *
 * append child to children list
 *
 */
void Module_appendChild(Module * m, Module * child){
    child->next = m->children_modules_list;
    m->children_modules_list = child;
    return;
}

/*
 *
 *
 *  free current module and all its children and brothers
 *
 *
 */
void Module_free(Module * m){
    /* todo later */
}

void string_split_for_module(char * input_string, char * splitted_[128], int32_t * n_){
    uint32_t n = 0; // split length
    int32_t j, start, k;
    char * t;
    /*
     *  split var_name. eg string/add splits to ["string", "add"]
     */
    start = 0;
    // split string
    for(j = 0; j < (uint32_t)strlen(input_string); j++){
        if(input_string[j] == '/'){
            /*char */ t = (char*)malloc(sizeof(char)*(j - start + 1));
            for(k = start; k < j; k++){
                t[k-start] = input_string[k];
            }
            t[k-start] = 0;
            start = j+1;
            splitted_[n] = t;
            n++; // increase size
        }
    }
    // append last
    t = (char*)malloc(sizeof(char)*(j - start + 1));
    for(k = start; k < j; k++){
        t[k-start] = input_string[k];
    }
    t[k-start] = 0;
    splitted_[n] = t;
    n++; // increase size
    
    *n_ = n++;
    return;
}

void VT_find(Variable_Table * vt, char * var_name, int32_t output[2], Module * module){
    char * splitted_[128];
    int32_t n = 0; // split length
    int32_t i, j;
    /*
     *  split var_name. eg string/add splits to ["string", "add"]
     */
    string_split_for_module(var_name, splitted_, &n);
    
    printf("# VT_find\n");
    for (i = 0; i < n; i++) {
        printf("%s \n", splitted_[i]);
    }
    
    /*
     *   get index
     *   global or local
     */
    if (n == 1) {
        Variable_Table_Frame * frame;
        // check local
        for (i = vt->length - 1; i >= 1; i--) {
            frame = vt->frames[i];
            for (j = frame->length-1; j >= 0; j--) {
                if (frame->var_names[j] == NULL) {
                    continue;
                }
                if(str_eq(frame->var_names[j], var_name)){
                    output[0] = i;
                    output[1] = j;
                    goto free_splitted;
                }
            }
        }
        // check global
        module = GLOBAL_MODULE;
        goto check_variable_in_module;
        
        output[0] = -1;
        output[1] = -1;
        goto free_splitted;
    }
    /*
     * module
     */
    for (i = 0; i < n - 1; i++) {
        int find_module = 0;
        module = module->children_modules_list; // get children
        while (module != NULL) {
            if (str_eq(splitted_[i], module->module_as_name)) {
                // find module
                find_module = true;
                break;
            }
            module = module->next; // check next
        }
        if (find_module) {
            continue;
        }
        // didn't find corresponding module
        printf("ERROR: didn't find corresponding module: %s in: %s\n", splitted_[i], var_name);
        output[0] = -1;
        output[1] = -1;
        goto free_splitted;
    }
    /*
     *
     * todo: global/x problem. support global module.现在不支持
     *
     */
check_variable_in_module:;
    // find module.
    Variable_Table_Frame * f = vt->frames[0]; // get global frames
    for (i = 0; i < module->length; i++) {
        if (str_eq(f->var_names[module->vtf_offset[i]], splitted_[n - 1])) {
            // find corresponding variable
            output[0] = 0;
            output[1] = module->vtf_offset[i];
            goto free_splitted;
        }
    }
    // didn't find corresponding module
    printf("ERROR: didn't find corresponding variable: %s in: %s\n", splitted_[n - 1], var_name);
    output[0] = -1;
    output[1] = -1;
    

free_splitted: // free char* in splitted_ and return.
    for (i = 0; i < n; i++) {
        free(splitted_[i]);
    }
    return;
}

#endif














