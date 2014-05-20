//
//  garbage.h
//  walley
//
//  Created by WangYiyi on 4/30/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_garbage_h
#define walley_garbage_h
#include "compiler.h"
/*
 free
 */
void Object_free(Object * o){
    unsigned long i, length, size;
    Table_Pair * temp;
    Table_Pair * p;
    Environment * env;
    if(o->use_count == 0){
        // free
        switch (o->type){
            case NULL_:
                return; // cannot free null;
                // null will be stored in string_table(constant_table) index0;
            case INTEGER: case DOUBLE: case RATIO:
                free(o);
                return;
            case STRING:
                free(o->data.String.v);
                free(o);
                return;
            case PAIR:
                // decrement the use count
                o->data.Pair.car->use_count-=1;
                o->data.Pair.cdr->use_count-=1;
                
                Object_free(o->data.Pair.car);
                Object_free(o->data.Pair.cdr);
                
                free(o);
                return;
            case USER_DEFINED_LAMBDA:
                // Env_free(o->data.User_Defined_Lambda.env);
                env = o->data.User_Defined_Lambda.env;
                length = env->length;
                for (i = 0; i < length; i++) {
                    env->frames[i]->use_count--;
                    EF_free(env->frames[i]);
                }
                free(env->frames);
                free(env);
                free(o);
                return;
            case BUILTIN_LAMBDA:
                return; // cannt free builtin lambda
            case VECTOR:
                length = o->data.Vector.length;
                Object ** v = o->data.Vector.v;
                for(i = 0; i < length; i++){
                    v[i]->use_count--; // decrease use count
                    Object_free(v[i]);
                }
                free(o->data.Vector.v);
                free(o);
                return;
            case TABLE:
                size = o->data.Table.size;
                // length = o->data.Table.length;
                for(i = 0; i < size; i++){
                    if(o->data.Table.vec[i]){ // exist
                        p = o->data.Table.vec[i]; // get Table_Pair;
                        while(p != NULL){
                            p->key->use_count--;     // decrease use_count
                            Object_free(p->key);     // free key
                            p->value->use_count--;   // decrease use_count
                            Object_free(p->value);   // free value
                            temp = p;
                            p = p->next;
                            free(temp); // free that Table_Pair
                        }
                    }
                }
                free(o->data.Table.vec); // free table vector
                free(o);
                return;
            default:
                printf("ERROR: Object_free invalid data type\n");
                return;
        }
    }
    //else{
    //  o->use_count--; // decrease use_count
    //}
}

#endif
