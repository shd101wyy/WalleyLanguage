//
//  ratio.h
//  walley
//
//  Created by WangYiyi on 4/29/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_ratio_h
#define walley_ratio_h
#include "data_type.h"

#define numer(v) (v)->data.Ratio.numer
#define denom(v) (v)->data.Ratio.denom

int64_t gcd(int64_t a, int64_t b){
    int64_t temp;
    while(b!=0)
    {
        temp = a;
        a = b;
        b = temp%b;
    }
    return a;
}
/*
    create ratio data type
 */
Object * Object_initRatio(int64_t numer, int64_t denom){
    int64_t g;
    Object * o;
    if (denom == 1) {
    it_is_integer:
            o = allocateObject();
            o->type = INTEGER;
            o->data.Integer.v = numer;
            return o;
    }
    else{
        g = gcd(numer, denom);
        numer = numer / g;
        denom = denom / g;
        if (denom == 1) goto it_is_integer;
        
        o = allocateObject();
        o->type = RATIO;
        o->data.Ratio.numer = numer;
        o->data.Ratio.denom = denom;
        return o;
    }
}
/*
// fraction arithematic
Object * add_rat(Object* x, Object* y){
    return Object_initRatio( numer(x)*denom(y)+numer(y)*denom(x) , denom(x)*denom(y));
}
Object * sub_rat(Object* x, Object* y){
    return Object_initRatio( numer(x)*denom(y)-numer(y)*denom(x) , denom(x)*denom(y));
}
Object *  mul_rat(Object* x, Object* y){
    return Object_initRatio(numer(x)*numer(y), denom(x)*denom(y));
}
Object *  div_rat(Object* x, Object* y){
    return Object_initRatio(numer(x)*denom(y),denom(x)*numer(y));
}*/
Object * add_rat(int64_t numer_x, int64_t denom_x, int64_t numer_y, int64_t denom_y){
    return Object_initRatio( numer_x*denom_y+numer_y*denom_x , denom_x*denom_y);
}
Object * sub_rat(int64_t numer_x, int64_t denom_x, int64_t numer_y, int64_t denom_y){
    return Object_initRatio( numer_x*denom_y-numer_y*denom_x , denom_x*denom_y);
}
Object *  mul_rat(int64_t numer_x, int64_t denom_x, int64_t numer_y, int64_t denom_y){
    return Object_initRatio(numer_x*numer_y, denom_x*denom_y);
}
Object *  div_rat(int64_t numer_x, int64_t denom_x, int64_t numer_y, int64_t denom_y){
    return Object_initRatio(numer_x*denom_y,denom_x*numer_y);
}

#if RATIO_DEBUG
void ratio_debug(Object * x){
    printf("%ld/%ld\n", x->data.Ratio.numer, x->data.Ratio.denom);
}
void number_debug(Object * x){
    switch (x->type) {
        case INTEGER:
            printf("Integer %ld\n", x->data.Integer.v);
            break;
        case DOUBLE:
            printf("Float %lf\n", x->data.Double.v);
            break;
        case RATIO:
            printf("Ratio %ld/%ld\n", x->data.Ratio.numer, x->data.Ratio.denom);
            break;
        default:
            printf("Invalid Number");
            break;
    }
}
#endif

#endif
