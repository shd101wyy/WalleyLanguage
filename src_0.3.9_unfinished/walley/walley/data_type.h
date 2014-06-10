//
//  data_type.h
//  walley
//
//  Created by WangYiyi on 6/10/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_data_type_h
#define walley_data_type_h


#include "all_functions_header.h"

/*
 *
 * several global strings
 *
 */
Data * STRING_proto, 
     * STRING_type,
     * STRING_Integer,
     * STRING_Float,
     * STRING_List,
     * STRING_Vector,
     * STRING_Fn,
     * STRING_String,
     * STRING_Object,
     * STRING_num_add, // +
     * STRING_num_sub, // -
     * STRING_num_mul, // *
     * STRING_num_div,  // /
     * STRING_length
;


/*
 *
 * global data
 *
 */
Data * GLOBAL_NULL,
* GLOBAL_OBJECT,
* GLOBAL_INTEGER,
* GLOBAL_FLOAT,
* GLOBAL_STRING,
* GLOBAL_LIST,
* GLOBAL_BUILTIN_FN;

typedef enum{
    INTEGER,
    FLOAT,
    LIST,
    VECTOR,
    FN,
    STRING,
    OBJECT,
    NULL_,
    BUILTIN_FN
} Data_Type;

struct Data{
    uint16_t use_count;
    union{
        struct{
            int64_t v;
        } Integer;
        struct{
            double v;
        } Float;
        struct{
            Data * car;
            Data * cdr;
        } List;
        struct{
            Data ** v;
            uint32_t size;
        } Vector;
        struct{
            
        } Fn;
        struct{
            char * v;
        } String;
        struct{
            
        } Object;
        struct{
            
        } Null;
        struct{
            Data* (*func_ptr)(Data*, Data **, uint16_t); // function pointer
        } Builtin_Fn;
    }data;
    Data ** msgs;
    Data ** actions;
    uint16_t size;
    uint16_t length;
    Data_Type type;
};

Data * allocateData(){
    Data * d = malloc(sizeof(Data));
    return d;
}

void Object_setNewSlot(Data * object, Data * msg, Data * action){
    if (object->length == object->size) { // realloc
        object->size *= 2;
        object->msgs = realloc(object->msgs, sizeof(Data *) * object->size);
        object->actions = realloc(object->actions, sizeof(Data *) * object->size);
    }
    object->msgs[object->length] = msg;
    object->actions[object->length] = action;
    object->length++;
    
    msg->use_count++;
    action->use_count++;
}

uint8_t Object_checkMsgExist(Data * object, Data * msg){
    uint16_t i;
    for (i = 0; i < object->length; i++) {
        if (object->msgs[i] == msg) {
            return 1;
        }
    }
    return 0;
}
/*
 *
 * initialize Null ()  GLOBAL_NULL
 * cannot be freed
 *
 *
 *
 */
Data * Data_initNULL(){
    Data * d = malloc(sizeof(Data));
    d->actions = NULL;
    d->msgs = NULL;
    d->size = 0;
    d->length = 0;
    d->use_count = 1; // cannot be freed
    
    d->type = NULL_;
    return d;
}
/*
 *
 * initialize Object  GLOBAL_OBJECT
 * cannot be freed
 *############################
 * properties
 * proto : () Null
 * type : "Object"
 : clone :
 */
Data * Data_initObject(){
    Data * o = malloc(sizeof(Data));
    o->size = 8;
    o->length = 0;
    o->msgs = malloc(sizeof(Data*) * o->size);
    o->actions = malloc(sizeof(Data*) * o->size);
    o->use_count = 1; // cannot be freed
    
    Object_setNewSlot(o, STRING_proto, GLOBAL_NULL);  // set proto
    Object_setNewSlot(o, STRING_type, STRING_Object); // set type
    /*
     * TODO : implement builtin functions ... like clone
     */
    
    o->type = OBJECT;
    return o;
}






/*
 *
 *  initialize Vector GLOBAL_VECTOR
 *  cannot be freed
 *############################
 *  properties
 *  proto : Object
 *  type  : "Vector"
 */
Data * Data_initVector(){
    Data * o = malloc(sizeof(Data));
    o->size = 8;
    o->length = 0;
    o->msgs = malloc(sizeof(Data*) * o->size);
    o->actions = malloc(sizeof(Data*) * o->size);
    o->use_count = 1; // cannot be freed
    
    Object_setNewSlot(o, STRING_proto, GLOBAL_OBJECT);  // set proto
    Object_setNewSlot(o, STRING_type, STRING_Vector);    // set type
    
    o->type = OBJECT;
    return o;
}


/*
 *
 *  initialize Fn GLOBAL_FN
 *  cannot be freed
 *############################
 *  properties
 *  proto : Object
 *  type  : "Fn"
 */
Data * Data_initFn(){
    Data * o = malloc(sizeof(Data));
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





#endif









