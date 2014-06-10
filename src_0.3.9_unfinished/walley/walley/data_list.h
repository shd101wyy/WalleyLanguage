//
//  data_list.h
//  walley
//
//  Created by WangYiyi on 6/10/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_data_list_h
#define walley_data_list_h
#include "data_string.h"

/*
 *
 *  initialize List GLOBAL_LIST
 *  cannot be freed
 *======================================
 *  properties
 *  proto : Object
 *  type  : "List"
 */
Data * Data_initList(){
    Data * o = malloc(sizeof(Data));
    o->size = 8;
    o->length = 0;
    o->msgs = malloc(sizeof(Data*) * o->size);
    o->actions = malloc(sizeof(Data*) * o->size);
    o->use_count = 1; // cannot be freed
    
    Object_setNewSlot(o, STRING_proto, GLOBAL_OBJECT);  // set proto
    Object_setNewSlot(o, STRING_type, STRING_List);    // set type
    
    o->type = OBJECT;
    return o;
}

/*
 *
 * initialize list instance
 *======================================
 *  properties
 *  proto  : List GLOBAL_LIST
 *  type   : "List"
 *=======================================
 *  special
 *  Data * car
 *  Data * cdr
 */
Data * cons(Data * car, Data * cdr){
    Data * o = malloc(sizeof(Data));
    o->use_count = 0;
    o->size = 2;
    o->length = 0;
    o->msgs = malloc(sizeof(Data*) * o->size);
    o->actions = malloc(sizeof(Data*) * o->size);
    
    o->data.List.car = car;
    o->data.List.cdr = cdr;
    
    car->use_count++;
    cdr->use_count++;
    
    Object_setNewSlot(o, STRING_proto, GLOBAL_LIST);
    Object_setNewSlot(o, STRING_type, STRING_List);
    
    o->type = LIST;
    return o;
}





#endif
