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

    // set frame0
    VT_push(vt, 0, "cons");
    VT_push(vt, 0, "car");
    VT_push(vt, 0, "cdr");
    VT_push(vt, 0, "+");
    VT_push(vt, 0, "-");
    VT_push(vt, 0, "*");
    VT_push(vt, 0, "/");
    VT_push(vt, 0, "vector!");
    VT_push(vt, 0, "vector");
    VT_push(vt, 0, "vector-length");
    
    VT_push(vt, 0, "vector-push!");
    VT_push(vt, 0, "vector-pop!");
    VT_push(vt, 0, "=");
    VT_push(vt, 0, "<");
    VT_push(vt, 0, "<=");
    VT_push(vt, 0, "eq?");
    VT_push(vt, 0, "eval");  // 16
    VT_push(vt, 0, "exit");  // 17
    VT_push(vt, 0, ">");// 18
    VT_push(vt, 0, ">="); // 19
    
    VT_push(vt, 0, "parse"); // 20
    VT_push(vt, 0, "random"); // 21
    VT_push(vt, 0, "strcmp");
    VT_push(vt, 0, "string-slice");
    VT_push(vt, 0, "string-length");
    VT_push(vt, 0, "string-append");
    VT_push(vt, 0, "table");
    VT_push(vt, 0, "table-keys");
    VT_push(vt, 0, "table-delete");
    VT_push(vt, 0, "file-read");
    
    VT_push(vt, 0, "file-write");
    VT_push(vt, 0, "sys-argv");
    VT_push(vt, 0, "int->string");
    VT_push(vt, 0, "float->string");
    VT_push(vt, 0, "input");
    VT_push(vt, 0, "display");
    VT_push(vt, 0, "string->int");
    VT_push(vt, 0, "string->float");
    VT_push(vt, 0, "ratio?");
    VT_push(vt, 0, "numer");

    VT_push(vt, 0, "denom");
    VT_push(vt, 0, "gensym");
    VT_push(vt, 0, "table-add-tag");
    VT_push(vt, 0, "table-tag");
    VT_push(vt, 0, "typeof");  // 44
    VT_push(vt, 0, "math-cos");  // 45
    VT_push(vt, 0, "math-sin");
    VT_push(vt, 0, "math-tan");
    VT_push(vt, 0, "math-acos");
    VT_push(vt, 0, "math-asin");
    
    VT_push(vt, 0, "math-atan");  // 50
    VT_push(vt, 0, "math-cosh");  // 51
    VT_push(vt, 0, "math-sinh");  // 52
    VT_push(vt, 0, "math-tanh");  // 53
    VT_push(vt, 0, "math-log");  // 54
    VT_push(vt, 0, "math-exp");  // 55
    VT_push(vt, 0, "math-log10");  // 56
    VT_push(vt, 0, "math-pow");  // 57
    VT_push(vt, 0, "math-sqrt");  // 58
    VT_push(vt, 0, "math-ceil");  // 59

    VT_push(vt, 0, "math-floor");  // 60
    VT_push(vt, 0, "string-find");  // 61
    VT_push(vt, 0, "string-replace");  // 62
    VT_push(vt, 0, "apply");   // 63
    VT_push(vt, 0, "vector-slice"); // 64
    VT_push(vt, 0, "set-car!"); // 65
    VT_push(vt, 0, "set-cdr!"); // 66
    return vt;
}

void Variable_Table_push_frame(Variable_Table * vt, Variable_Table_Frame * vtf){
    vtf->use_count++; // increase use_count of vtf
    
    vt->frames[vt->length] = vtf;
    vt->length++;
}

void VT_find(Variable_Table * vt, char * var_name, int32_t output[2]){
    int32_t i, j;
    Variable_Table_Frame * frame;
    for (i = vt->length - 1; i >= 0; i--) {
        frame = vt->frames[i];
        for (j = frame->length-1; j >= 0; j--) {
            if(str_eq(frame->var_names[j], var_name)){
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

#endif














