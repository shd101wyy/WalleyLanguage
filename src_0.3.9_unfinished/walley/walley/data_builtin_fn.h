//
//  builtin_fn.h
//  walley
//
//  Created by WangYiyi on 6/10/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_builtin_fn_h
#define walley_builtin_fn_h
#include "data_float.h"


/*
 *
 *  initialize Builtin_Fn GLOBAL_BUILTIN_FN
 *  cannot be freed
 *############################
 *  properties
 *  proto : Object
 *  type  : "Fn"
 */
Data * Data_initBuiltinFn(Data * o){
    o->size = 8;
    o->length = 0;
    o->msgs = malloc(sizeof(Data*) * o->size);
    o->actions = malloc(sizeof(Data*) * o->size);
    o->use_count = 1; // cannot be freed
    
    Object_setNewSlot(o, STRING_proto, GLOBAL_OBJECT);  // set proto
    Object_setNewSlot(o, STRING_type, STRING_Fn);    // set type
    
    o->type = OBJECT;
    return o;
}

/*
 *
 *  initialize builtin fn instance
 *#####################################
 *  properties
 *  proto : Fn GLOBAL_BUILTIN_FN
 *  type  : "Fn"
 */
Data * BuiltinFn_initInstance(Data* (*func_ptr)(Data*, Data **, uint16_t)){
    Data * o = malloc(sizeof(Data));
    o->use_count = 0;
    o->size = 2;
    o->length = 0;
    o->msgs = malloc(sizeof(Data*) * o->size);
    o->actions = malloc(sizeof(Data*) * o->size);
    
    o->data.Builtin_Fn.func_ptr = func_ptr;
    
    Object_setNewSlot(o, STRING_proto, GLOBAL_BUILTIN_FN);
    Object_setNewSlot(o, STRING_type, STRING_Fn);
    
    o->type = BUILTIN_FN;

    return o;
}


#endif
