//
//  data_string.h
//  walley
//
//  Created by WangYiyi on 6/10/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_data_string_h
#define walley_data_string_h

#include "data_builtin_fn.h"

/*
 *
 *  initialize String GLOBAL_STRING
 *  cannot be freed
 *======================================
 *  properties
 *  proto : Object
 *  type  : "String"
 *  append  :
 *  find    :
 *  replace :
 *  at      :
 */
Data * Data_initString(Data * o){ // pass GLOBAL_STRING as parameter
    o->size = 8;
    o->length = 0;
    o->msgs = malloc(sizeof(Data*) * o->size);
    o->actions = malloc(sizeof(Data*) * o->size);
    o->use_count = 1; // cannot be freed
    
    Object_setNewSlot(o, STRING_proto, GLOBAL_OBJECT);  // set proto
    Object_setNewSlot(o, STRING_type, STRING_String);    // set type
    /*
     * TODO : implement builtin_fn for string
     *
     */
    
    o->type = OBJECT;
    return o;
}

/*
 *
 *  initialize string instance
 *======================================
 *  properties
 *  proto  : String GLOBAL_STRING
 *  type   : "String"
 *  length : length of string
 *======================================
 *  special
 *  char * v
 */
Data * String_initInstance(char * v){
    Data * o = malloc(sizeof(Data));
    o->use_count = 0;
    o->size = 4;
    o->length = 0;
    o->msgs = malloc(sizeof(Data*) * o->size);
    o->actions = malloc(sizeof(Data*) * o->size);
    
    uint64_t length =  strlen(v);
    o->data.String.v = malloc(sizeof(char) * (length + 1));
    strcpy(o->data.String.v, v);
    
    
    Object_setNewSlot(o, STRING_proto, GLOBAL_STRING);
    Object_setNewSlot(o, STRING_type, STRING_String);
    Object_setNewSlot(o, STRING_length, Integer_initInstance(length));
    
    o->type = STRING;
    return o;
}

Data * String_initInstanceHelper(Data * o, char * v){
    // o->use_count = use_count;
    o->size = 4;
    o->length = 0;
    o->msgs = malloc(sizeof(Data*) * o->size);
    o->actions = malloc(sizeof(Data*) * o->size);
    
    uint64_t length =  strlen(v);
    o->data.String.v = malloc(sizeof(char) * (length + 1));
    strcpy(o->data.String.v, v);
    
    
    Object_setNewSlot(o, STRING_proto, GLOBAL_STRING);
    Object_setNewSlot(o, STRING_type, STRING_String);
    Object_setNewSlot(o, STRING_length, Integer_initInstance(length));
    
    o->type = STRING;
    return o;

}
#endif
