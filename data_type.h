//
//  data_type.c
//  walley
//
//  Created by WangYiyi on 4/27/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//
/*
 walley language 0.3 by Yiyi Wang @ 2014
 Released under MIT Licenses
 data_type.c
 */

#ifndef DATA_TYPE_C
#define DATA_TYPE_C
#include "debug.h"

uint8_t COMPILATION_MODE = 0; // if under compilation mode, no print necessary

typedef struct Object Object;
typedef struct Continuation_Saved_State Continuation_Saved_State;
typedef struct Table_Pair Table_Pair;

typedef struct Environment Environment;
typedef struct Environment_Frame Environment_Frame;

void Env_free(Environment * env);
Environment * Env_init_with_size(int size);
Environment_Frame * EF_init_with_size(int size);

void Object_free(Object * o);

// lexer
typedef struct Lexer Lexer;
Lexer* lexer(char * input_string);

// parser
Object * parser(Lexer * le);

static Object * GLOBAL_NULL;
static Object * GLOBAL_TRUE;
static Object * QUOTE_STRING;
static Object * UNQUOTE_STRING;
static Object * UNQUOTE_SPLICE_STRING;
static Object * QUASIQUOTE_STRING;
static Object * CONS_STRING;
static Object * DEF_STRING;
static Object * SET_STRING;
static Object * LAMBDA_STRING;

static Object * INTEGER_STRING;
static Object * FLOAT_STRING;
static Object * RATIO_STRING;
static Object * STRING_STRING;
static Object * PAIR_STRING;
static Object * VECTOR_STRING;
static Object * TABLE_STRING;
static Object * CLONE_STRING;

// for object
static Object * STRING_proto;
static Object * STRING_type;
static Object * STRING_object;

static Object * SYS_ARGV;

//static int * INSTRUCTIONS;
//static int INSTRUCTIONS_LENGTH;
void Object_free(Object * o);
/* data types */
typedef enum {
    NULL_,
	INTEGER,
	DOUBLE_,
	RATIO,
	STRING,
	PAIR,
	USER_DEFINED_LAMBDA,
	BUILTIN_LAMBDA,
	VECTOR,
	TABLE,
    OBJECT,
    FILE_,
    CONTINUATION
} DataType;
struct  Table_Pair{ // used for table
  Object * key;          // key
  Object * value;        // value
  Table_Pair * next;     // next
};
/*
 several data types
 */
struct Object {
    DataType type;        // 4 bytes
    uint32_t use_count;   // 4 bytes
    union {
        struct {      // 8 bytes 
            int64_t v; // 将来会只用long
        } Integer;
        struct {      // 8 bytes
            double v;
        } Double;
        struct{       // 16 bytes
            int64_t denom;
            int64_t numer;
         } Ratio;
        struct {      // 8 + 8 + 8 + 8 = 32 bytes
            uint64_t size;
            uint64_t length;
            Table_Pair **vec; // array to save Table_Pairs
            Object * proto;   // for object
        } Table;
        struct {     //  16 bytes
            char * v;
            uint64_t length;
        } String;
        struct {    //   16 bytes
            Object * car;
            Object * cdr;
        } Pair;
        struct {   // 2 + 8 + 8 + 1 = 24 bytes
            uint8_t param_num;
            int8_t variadic_place;
            uint64_t start_pc;
            Environment * env;
            uint8_t frame_size;
        } User_Defined_Lambda;
        struct {   //  8 + 8 + 8 + 1 => 32 bytes
            Object ** v;   // array of pointers
            uint64_t size;      // size of allocated vector
            uint64_t length;    // length
            uint8_t resizable : 1;  // is this vector resizable
        } Vector;
        struct {          // builtin lambda
            Object* (*func_ptr)(Object**, uint32_t); // function pointer
        } Builtin_Lambda;
        struct{
            FILE * file_ptr;
        } File;
        struct {         // continuation
            // TODO : add structures.
            uint64_t pc;
            Environment * env;
            Continuation_Saved_State * state;
        } Continuation;
        /*
        struct {
            Object ** msgs;
            Object ** actions;
            uint16_t size;
            uint16_t length;
            uint32_t object_id;
        } Object_;
        
        struct{
            int64_t v;           // value
            Object * msgs[1];    // used to save proto
            Object * actions[1];
        } Integer_Object; // use this in the future?
         */
    } data;
};

struct Continuation_Saved_State{
    Environment_Frame * builtin_primitive_procedure_stack;
    
    Environment ** continuation_env;
    uint16_t continuation_env_length;
    
    uint64_t * continuation_return_pc;
    int16_t continuation_return_pc_length;
    
    Environment_Frame ** frames_list;
    uint16_t frames_list_length;
    
    Object ** functions_list;
    uint16_t functions_list_length;
};



#define OBJECT_SIZE sizeof(Object)

#define vector_Get(v_, index)  ((v_)->data.Vector.v[(index)])
#define vector_Length(v) ((v)->data.Vector.length)
#define vector_Size(v) ((v)->data.Vector.size)
#define vector_Set(v_, index, o_) ((v_)->data.Vector.v[(index)] = o_)

#define string_Length(v_) ((v_)->data.String.length)
#define vector_Push(v_, val_)  \
vector_Set((v_), vector_Length((v_)),  (val_)); \
(v_)->data.Vector.length++ ; \

#define car(v) ((v)->data.Pair.car)
#define cdr(v) ((v)->data.Pair.cdr)
#define cadr(v) (car(cdr((v))))
#define caddr(v) (car(cdr(cdr((v)))))
#define cadddr(v) (car(cdr(cdr(cdr((v))))))
#define cdddr(v) (cdr(cdr(cdr((v)))))
#define cddr(v) (cdr(cdr((v))))

/*
 allocate object
 */
Object * allocateObject(){
    Object * o = (Object*)malloc(OBJECT_SIZE);
    if(o == NULL){
        printf("ERROR:Out of memory\n");
        exit(1);
    }
    o->use_count = 0;
    return o;
}
/*
 initialize integer
 */
Object * Object_initInteger(int64_t v){
    Object * o = allocateObject();
    o->type = INTEGER;
    o->data.Integer.v = v;
    return o;
}
/*
 initialize double
 */
Object * Object_initDouble(double v){
    Object * o = allocateObject();
    o->type = DOUBLE_;
    o->data.Double.v = v;
    return o;
}
/*
 initialize string
 */
Object * Object_initString(char * v, uint64_t string_length){
    Object * o = allocateObject();
    o->type = STRING;
    o->data.String.v = (char*)malloc(sizeof(char)*(string_length + 1));
    if(o->data.String.v == NULL)
    {
        printf("ERROR:Out of memory\n");
        exit(1);
    }
    strcpy(o->data.String.v, v);
    o->data.String.length = string_length;
    // o->use_count = 0;  // already set to 0 when allocateObject()
    return o;
}
/*
 initialize vector
 default size 32
 */

Object * Object_initVector(char resizable, uint64_t size){
    Object * o = allocateObject();
    o->type = VECTOR;
    o->data.Vector.size = size;
    o->data.Vector.length = 0;
    o->data.Vector.resizable = resizable;
    o->data.Vector.v = (Object**)malloc(sizeof(Object*) * (size)); // array of pointers
    
    if(o->data.Vector.v == NULL){
        printf("ERROR:Out of memory\n");
        exit(1);
    }
    return o;
}
/*
 initialize nil
 */
Object * Object_initNull(){
    Object * o = allocateObject();
    o->type = NULL_;
    return o;
}


/*
 initialize user defined lambda
 */
Object * Object_initUserDefinedLambda(uint8_t param_num, int8_t variadic_place, uint64_t start_pc, Environment * env, uint8_t frame_length){
    Object * o = allocateObject();
    o->type = USER_DEFINED_LAMBDA;
    o->data.User_Defined_Lambda.param_num = param_num;
    o->data.User_Defined_Lambda.variadic_place = variadic_place;
    o->data.User_Defined_Lambda.start_pc = start_pc;
    o->data.User_Defined_Lambda.env = env;
    o->data.User_Defined_Lambda.frame_size = frame_length;
    return o;
}
Object * Object_initBuiltinLambda(Object* (*func_ptr)(Object **, uint32_t)){
    Object * o = allocateObject();
    o->type = BUILTIN_LAMBDA;
    o->data.Builtin_Lambda.func_ptr = func_ptr;
    o->use_count = 1;
    return o;
}

/*
 cons
 */
Object * cons(Object * car, Object * cdr){
    Object * o = allocateObject();
    
    // 只有 builtin_procedure 需要 增加
    car->use_count += 1; // increase use_count
    cdr->use_count += 1; // increase use_count
    
    o->type = PAIR;
    o->data.Pair.car = car;
    o->data.Pair.cdr = cdr;
    return o;
}
/*
 * HashTable functions
 * quick hash function
 * djb2
 */
Object * Object_initTable(uint64_t size){
    Object * o = allocateObject();
    o->type = TABLE;
    o->data.Table.length = 0;
    o->data.Table.size = size;
    o->data.Table.vec = (Table_Pair**)calloc(size, sizeof(Table_Pair*));
    o->data.Table.proto = NULL;
    // o->use_count = 0;
    return o;
}
uint64_t hash(Object * o, uint64_t size){
    if(o->type == STRING){
        char * str = o->data.String.v;
        uint64_t hash = 0;
        while (*(str)){
            hash = ((hash << 5) + hash) + *(str); // hash * 33 + c
            str++;
        }
        if(hash >= size) //size必须是>0的
            return hash % size;
        return hash;
    }
    else{
        uint64_t hash = (uint64_t)o;
        if(hash >= size) //size必须是>0的
            return hash % size;
        return hash;
    }
}

/*
 rehash function
 */
void rehash(Object * t){
    // make space to save keys
    uint64_t i = 0;
    uint64_t j = 0;
    uint64_t original_size = t->data.Table.size;
    Object * key;
    Object * value;
    uint64_t hash_value;
    Table_Pair * p;
    Table_Pair * temp;
	Object ** keys = (Object**)malloc(sizeof(Object) * original_size); //[original_size];
	Object ** values = (Object**)malloc(sizeof(Object) * original_size); // [original_size];
    Table_Pair * new_Table_Pair;
    
    for(i = 0; i < original_size; i++){
        p = t->data.Table.vec[i]; // get Table_Pair;
        if(p){
            while(p != NULL){
                keys[j] = p->key;     // save keys
                values[j] = p->value; // save values;
                temp = p;
                p = p->next;
                free(temp); // free that Table_Pair
                j++;
            }
        }
    }
    // free vec
    free(t->data.Table.vec);
    t->data.Table.size = t->data.Table.size * 2; // double size
    t->data.Table.vec = (Table_Pair**)calloc(t->data.Table.size, sizeof(Table_Pair*)); // realloc
    
    // rehash
    for(i = 0; i < j; i++){
        key = keys[i];
        value = values[i];
        // create new Table_Pair
        new_Table_Pair = (Table_Pair*)malloc(sizeof(Table_Pair));
        new_Table_Pair->key = key;
        new_Table_Pair->value = value;
        
        hash_value = hash(key, t->data.Table.size); // rehash
        if(t->data.Table.vec[hash_value] != NULL){ // already exist
            new_Table_Pair->next = t->data.Table.vec[hash_value];
            t->data.Table.vec[hash_value] = new_Table_Pair;
        }
        else{ // doesn't exist
            new_Table_Pair->next = NULL;
            t->data.Table.vec[hash_value] = new_Table_Pair;
        }
    }

	free(keys);
	free(values);
}

/*
 getval
 */
Object * Table_getval(Object * t, Object * key){
    uint64_t hash_value = hash(key, t->data.Table.size); // get hash value
    Table_Pair * table_pairs = t->data.Table.vec[hash_value]; // get pairs
    while(table_pairs!=NULL){
        if( table_pairs->key == key || (key->type == STRING && table_pairs->key->type == STRING && strcmp(key->data.String.v, table_pairs->key->data.String.v) == 0)){
            return table_pairs->value;
        }
        table_pairs = table_pairs->next;
    }
    // didnt find
    return GLOBAL_NULL;
}
/*
 setval
 */
void Table_setval(Object *t, Object * key, Object * value){
    if(t->data.Table.length / (double)t->data.Table.size >= 0.7) // rehash
        rehash(t);
    uint64_t hash_value = hash(key, t->data.Table.size);
    Table_Pair * table_pairs = t->data.Table.vec[hash_value];
    Table_Pair * new_table_pair;
    Table_Pair * temp_table_pair;
    new_table_pair = (Table_Pair*)malloc(sizeof(Table_Pair));
    new_table_pair->key = key;
    new_table_pair->value = value;
    
    // increase use count for value and key
    key->use_count++;
    value->use_count++;
    
    if(table_pairs){ // already existed
        // go over to check whether already exist
        temp_table_pair = table_pairs;
        while(temp_table_pair){
            if(temp_table_pair->key == key || (key->type == STRING && temp_table_pair->key->type == STRING && strcmp(temp_table_pair->key->data.String.v, key->data.String.v) == 0))
            {
                // key already exist
                // free old value
                temp_table_pair->value->use_count--;
                Object_free(temp_table_pair->value);
                
                // set new value
                temp_table_pair->value = value;
                value->use_count++;
                return;
            }
            temp_table_pair = temp_table_pair->next;
        }
        // add new value
        new_table_pair->next = table_pairs;
        t->data.Table.vec[hash_value] = new_table_pair;
    }
    else{ // doesn't exist
        new_table_pair->next = NULL;
        t->data.Table.vec[hash_value] = new_table_pair;
    }
    t->data.Table.length++; // increase length
}
/*
 return keys as list
 */
Object * table_getKeys(Object * t){
    Object * keys = GLOBAL_NULL;
    uint32_t i = 0;
    Table_Pair * p;
    for(i = 0; i < t->data.Table.size; i++){
        p = t->data.Table.vec[i]; // get table pair
        if(p){
            while(p != NULL){
                keys = cons(p->key, keys);
                p->key->use_count++; // increase use count
                p=p->next;
            }
        }
    }
    return keys;
}

// init Continuation
Object * Object_initContinuation(uint64_t pc, Environment * env){
    Object * o = allocateObject();
    o->type = CONTINUATION;
    o->data.Continuation.pc = pc;
    o->data.Continuation.env = env;
    o->data.Continuation.state = malloc(sizeof(Continuation_Saved_State));
    return o;
}




#endif




