//
//  walley.h
//  walley
//
//  Created by WangYiyi on 6/10/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_walley_h
#define walley_walley_h
#include "to_string.h"
void Walley_initGlobal(){
    // init global string
    STRING_proto = calloc(1, sizeof(Data));
    STRING_type = calloc(1, sizeof(Data));
    STRING_Integer = calloc(1, sizeof(Data));
    STRING_Float = calloc(1, sizeof(Data));
    STRING_List = calloc(1, sizeof(Data));
    STRING_Vector = calloc(1, sizeof(Data));
    STRING_Fn = calloc(1, sizeof(Data));
    STRING_String = calloc(1, sizeof(Data));
    STRING_Object = calloc(1, sizeof(Data));
    STRING_num_add = calloc(1, sizeof(Data));
    STRING_num_sub = calloc(1, sizeof(Data));
    STRING_num_mul = calloc(1, sizeof(Data));
    STRING_num_div = calloc(1, sizeof(Data));
    STRING_length = calloc(1, sizeof(Data));
    
    // init global data
    GLOBAL_OBJECT = calloc(1, sizeof(Data));
    GLOBAL_NULL = calloc(1, sizeof(Data));
    GLOBAL_INTEGER = calloc(1, sizeof(Data));
    GLOBAL_FLOAT = calloc(1, sizeof(Data));
    GLOBAL_STRING = calloc(1, sizeof(Data));
    GLOBAL_LIST = calloc(1, sizeof(Data));
    GLOBAL_VECTOR = calloc(1, sizeof(Data));
    GLOBAL_FN = calloc(1, sizeof(Data));
    GLOBAL_BUILTIN_FN = calloc(1, sizeof(Data));
    
    /*
     *
     *  set properties for global data
     *
     */
    Data_initObject(GLOBAL_OBJECT);
    Data_initNULL(GLOBAL_NULL);
    Data_initInteger(GLOBAL_INTEGER);
    Data_initFloat(GLOBAL_FLOAT);
    Data_initString(GLOBAL_STRING);
    Data_initList(GLOBAL_LIST);
    /*
     * TODO : init GLOBAL_VECTOR and GLOBAL_FN
     */
    Data_initBuiltinFn(GLOBAL_BUILTIN_FN);

    /*
     *
     * set properties for global string
     *
     */
    String_initInstanceHelper(STRING_proto, "proto");
    String_initInstanceHelper(STRING_type, "type");
    String_initInstanceHelper(STRING_Integer, "Integer");
    String_initInstanceHelper(STRING_Float, "Float");
    String_initInstanceHelper(STRING_List, "List");
    String_initInstanceHelper(STRING_Vector, "Vector");
    String_initInstanceHelper(STRING_Fn, "Fn");
    String_initInstanceHelper(STRING_String, "String");
    String_initInstanceHelper(STRING_Object, "Object");
    String_initInstanceHelper(STRING_num_add, "+");
    String_initInstanceHelper(STRING_num_sub, "-");
    String_initInstanceHelper(STRING_num_mul, "*");
    String_initInstanceHelper(STRING_num_div, "/");
    String_initInstanceHelper(STRING_length, "length");
    
    printf("%d\n", GLOBAL_OBJECT->use_count);
}


#endif
