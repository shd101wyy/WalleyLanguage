//
//  float.h
//  walley
//
//  Created by WangYiyi on 6/10/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_float_h
#define walley_float_h
#include "data_integer.h"

/*
 * builtin_fn +
 * (3.0:+ 4 5)
 */
Data * builtin_float_add(Data * self, Data ** params, uint16_t param_num){
    double d = 0.0;            // double result
    uint16_t i;
    d = self->data.Integer.v;
    for (i = 0;  i < param_num; i++) {
        Data * p = params[i];
        if (p->type == INTEGER) {
            d += p->data.Integer.v;
        }
        else{
            d += p->data.Float.v;
        }
    }
    return Float_initInstance(d);
}

/*
 * builtin_fn -
 * (3.0:- 4 5)
 */
Data * builtin_float_sub(Data * self, Data ** params, uint16_t param_num){
    double d = 0.0;            // double result
    uint16_t i;
    d = self->data.Integer.v;
    for (i = 0;  i < param_num; i++) {
        Data * p = params[i];
        if (p->type == INTEGER) {
            d -= p->data.Integer.v;
        }
        else{
            d -= p->data.Float.v;
        }
    }
    return Float_initInstance(d);
}
/*
 * builtin_fn *
 * (3.0:* 4 5)
 */
Data * builtin_float_mul(Data * self, Data ** params, uint16_t param_num){
    double d = 0.0;            // double result
    uint16_t i;
    d = self->data.Integer.v;
    for (i = 0;  i < param_num; i++) {
        Data * p = params[i];
        if (p->type == INTEGER) {
            d *= p->data.Integer.v;
        }
        else{
            d *= p->data.Float.v;
        }
    }
    return Float_initInstance(d);}
/*
 * builtin_fn /
 * (3.0:/ 4 5)
 */
Data * builtin_float_div(Data * self, Data ** params, uint16_t param_num){
    double d = 0.0;            // double result
    uint16_t i;
    d = self->data.Integer.v;
    for (i = 0;  i < param_num; i++) {
        Data * p = params[i];
        if (p->type == INTEGER) {
            d /= p->data.Integer.v;
        }
        else{
            d /= p->data.Float.v;
        }
    }
    return Float_initInstance(d);}

/*
 *
 *  initialize Float GLOBAL_FLOAT
 *  cannot be freed
 *############################
 *  properties
 *  proto : Object
 *  type  : "Float"
 *  +     : builtin +
 *  -     : builtin -
 *  *     : buitlin *
 *  /     : builtin /
 */
Data * Data_initFloat(Data * o){
    o->size = 8;
    o->length = 0;
    o->msgs = malloc(sizeof(Data*) * o->size);
    o->actions = malloc(sizeof(Data*) * o->size);
    o->use_count = 1; // cannot be freed
    
    Object_setNewSlot(o, STRING_proto, GLOBAL_OBJECT);  // set proto
    Object_setNewSlot(o, STRING_type, STRING_Float);    // set type
    Object_setNewSlot(o, STRING_num_add, BuiltinFn_initInstance(&builtin_float_add)); // +
    Object_setNewSlot(o, STRING_num_sub, BuiltinFn_initInstance(&builtin_float_sub)); // -
    Object_setNewSlot(o, STRING_num_mul, BuiltinFn_initInstance(&builtin_float_mul)); // *
    Object_setNewSlot(o, STRING_num_div, BuiltinFn_initInstance(&builtin_float_div)); // /

    
    o->type = OBJECT;
    return o;
}

/*
 *   float instance
 * =======================
 * proto : Float  ⬆︎(The Object above GLOBAL_FLOAT)
 * type  : "Float"
 * =======================
 * v value of that integer
 */

Data * Float_initInstance(double v){
    Data * o = malloc(sizeof(Data));
    o->use_count = 0;
    o->size = 2;
    o->length = 0;
    o->msgs = malloc(sizeof(Data*) * o->size);
    o->actions = malloc(sizeof(Data*) * o->size);
    
    o->data.Float.v = v;
    
    Object_setNewSlot(o, STRING_proto, GLOBAL_FLOAT);
    Object_setNewSlot(o, STRING_type, STRING_Float);
    
    o->type = FLOAT;
    return o;
}

#endif
