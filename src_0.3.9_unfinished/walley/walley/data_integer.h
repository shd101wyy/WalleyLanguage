//
//  integer.h
//  walley
//
//  Created by WangYiyi on 6/10/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_integer_h
#define walley_integer_h
#include "data_type.h"

/*
 * builtin_fn +
 * (3:+ 4 5)
 */
Data * builtin_integer_add(Data * self, Data ** params, uint16_t param_num){
    double d = 0.0;            // double result
    int64_t t;           // integer result
    uint16_t i;
    uint8_t double_ = 0; // not double
    t = self->data.Integer.v;
    for (i = 0;  i < param_num; i++) {
        Data * p = params[i];
        if (double_) {
            if (p->type == INTEGER) {
                d += p->data.Integer.v;
            }
            else{
                d += p->data.Float.v;
            }
        }
        else{
            if (p->type == INTEGER) {
                t += p->data.Integer.v;
            }
            else{
                double_ = 1;
                d = t + p->data.Float.v;
            }
        }
    }
    if (double_) {
        return Float_initInstance(d);
    }
    return Integer_initInstance(t);
}

/*
 * builtin_fn -
 * (3:- 4 5)
 */
Data * builtin_integer_sub(Data * self, Data ** params, uint16_t param_num){
    double d = 0.0;            // double result
    int64_t t;           // integer result
    uint16_t i;
    uint8_t double_ = 0; // not double
    t = self->data.Integer.v;
    for (i = 0;  i < param_num; i++) {
        Data * p = params[i];
        if (double_) {
            if (p->type == INTEGER) {
                d -= p->data.Integer.v;
            }
            else{
                d -= p->data.Float.v;
            }
        }
        else{
            if (p->type == INTEGER) {
                t -= p->data.Integer.v;
            }
            else{
                double_ = 1;
                d = t - p->data.Float.v;
            }
        }
    }
    if (double_) {
        return Float_initInstance(d);
    }
    return Integer_initInstance(t);
}
/*
 * builtin_fn *
 * (3:* 4 5)
 */
Data * builtin_integer_mul(Data * self, Data ** params, uint16_t param_num){
    double d = 0.0;            // double result
    int64_t t;           // integer result
    uint16_t i;
    uint8_t double_ = 0; // not double
    t = self->data.Integer.v;
    for (i = 0;  i < param_num; i++) {
        Data * p = params[i];
        if (double_) {
            if (p->type == INTEGER) {
                d *= p->data.Integer.v;
            }
            else{
                d *= p->data.Float.v;
            }
        }
        else{
            if (p->type == INTEGER) {
                t *= p->data.Integer.v;
            }
            else{
                double_ = 1;
                d = t * p->data.Float.v;
            }
        }
    }
    if (double_) {
        return Float_initInstance(d);
    }
    return Integer_initInstance(t);
}
/*
 * builtin_fn /
 * (3:/ 4 5)
 */
Data * builtin_integer_div(Data * self, Data ** params, uint16_t param_num){
    double d = 0.0;            // double result
    int64_t t;           // integer result
    uint16_t i;
    uint8_t double_ = 0; // not double
    t = self->data.Integer.v;
    for (i = 0;  i < param_num; i++) {
        Data * p = params[i];
        if (double_) {
            if (p->type == INTEGER) {
                d /= p->data.Integer.v;
            }
            else{
                d /= p->data.Float.v;
            }
        }
        else{
            if (p->type == INTEGER) {
                t /= p->data.Integer.v;
            }
            else{
                double_ = 1;
                d = t / p->data.Float.v;
            }
        }
    }
    if (double_) {
        return Float_initInstance(d);
    }
    return Integer_initInstance(t);
}

/*
 *
 *  initialize Integer GLOBAL_INTEGER
 *  cannot be freed
 *############################
 *  properties
 *  proto : Object
 *  type  : "Integer"
 *  +     : builtin_fn +
 *  -     : builtin_fn -
 *  *     : builtin_fn *
 *  /     : builtin_fn /
 *
 */
Data * Data_initInteger(){
    Data * o = malloc(sizeof(Data));
    o->size = 8;
    o->length = 0;
    o->msgs = malloc(sizeof(Data*) * o->size);
    o->actions = malloc(sizeof(Data*) * o->size);
    o->use_count = 1; // cannot be freed
    
    Object_setNewSlot(o, STRING_proto, GLOBAL_OBJECT);  // set proto
    Object_setNewSlot(o, STRING_type, STRING_Integer);  // set type
    Object_setNewSlot(o, STRING_num_add, BuiltinFn_initInstance(&builtin_integer_add)); // +
    Object_setNewSlot(o, STRING_num_sub, BuiltinFn_initInstance(&builtin_integer_sub)); // -
    Object_setNewSlot(o, STRING_num_mul, BuiltinFn_initInstance(&builtin_integer_mul)); // *
    Object_setNewSlot(o, STRING_num_div, BuiltinFn_initInstance(&builtin_integer_div)); // /
    
    o->type = OBJECT;
    return o;
}

/*
 *   integer instance
 * =======================
 * proto : Integer  ⬆︎(The Object above GLOBAL_INTEGER)
 * type  : "Integer"
 * =======================
 * v value of that integer
 */

Data * Integer_initInstance(int64_t v){
    Data * o = malloc(sizeof(Data));
    o->use_count = 0;
    o->size = 2;
    o->length = 0;
    o->msgs = malloc(sizeof(Data*) * o->size);
    o->actions = malloc(sizeof(Data*) * o->size);
 
    o->data.Integer.v = v;
    
    Object_setNewSlot(o, STRING_proto, GLOBAL_INTEGER);
    Object_setNewSlot(o, STRING_type, STRING_Integer);
    
    o->type = INTEGER;
    return o;
}


#endif
