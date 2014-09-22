#ifndef builtin_procedures_h
#define builtin_procedures_h
#include "to_string.h"
#include <math.h>

static uint32_t GEN_SYM_COUNT = 0;
/*
 builtin lambdas
 */
// 0 cons
Object *builtin_cons(Object ** params, uint32_t param_num){
    Object * o = allocateObject();
    Object * car = params[0];
    Object * cdr = params[1];
    o->use_count = 0;
    o->type = PAIR;
    o->data.Pair.car = car;
    o->data.Pair.cdr = cdr;
    car->use_count++;
    cdr->use_count++;
    return o;
}
// 1 car
Object *builtin_car(Object ** params, uint32_t param_num){
    return car(params[0]);
}
// 2 cdr
Object *builtin_cdr(Object ** params, uint32_t param_num){
    return cdr(params[0]);
}
// 3 +
Object * add_2(Object * p1, Object * p2){
    switch (p1->type) {
        case INTEGER:
            switch (p2->type) {
                case INTEGER:
                    return Object_initInteger(p1->data.Integer.v + p2->data.Integer.v);
                case DOUBLE_:
                    return Object_initDouble(p1->data.Integer.v + p2->data.Double.v);
                case RATIO:
                    return add_rat(p1->data.Integer.v, 1,
                                   p2->data.Ratio.numer, p2->data.Ratio.denom);
                default:
                    printf("ERROR: + invalid data type");
                    return GLOBAL_NULL;
            }
            break;
        case DOUBLE_:
            switch (p2->type) {
                case INTEGER:
                    return Object_initDouble(p1->data.Double.v + p2->data.Integer.v);
                case DOUBLE_:
                    return Object_initDouble(p1->data.Double.v + p2->data.Double.v);
                case RATIO:
                    return Object_initDouble(p1->data.Double.v + p2->data.Ratio.numer/p2->data.Ratio.denom);
                default:
                    printf("ERROR: + invalid data type");
                    return GLOBAL_NULL;;
            }
            break;
        case RATIO:
            switch (p2->type) {
                case INTEGER:
                    return add_rat(p1->data.Ratio.numer, p1->data.Ratio.denom, p2->data.Integer.v, 1);
                case DOUBLE_:
                    return Object_initDouble(p1->data.Ratio.numer/p1->data.Ratio.denom + p2->data.Double.v);
                case RATIO:
                    return add_rat(p1->data.Ratio.numer, p1->data.Ratio.denom, p2->data.Ratio.numer, p2->data.Ratio.denom);
                default:
                    printf("ERROR: + invalid data type");
                    return GLOBAL_NULL;
            }
            break;
        default:
            printf("ERROR: + invalid data type");
            return GLOBAL_NULL;
    }
}
Object *builtin_add(Object ** params, uint32_t param_num){
    Object * return_val = params[0];
    Object *p1, * p2;
    int32_t i;
    for (i = 1; i < param_num; i++) {
        p2 = params[i];
        p1 = return_val;
        return_val = add_2(return_val, p2);
        Object_free(p1);
    }
    return return_val;
}
Object * sub_2(Object * p1, Object * p2){
    switch (p1->type) {
        case INTEGER:
            switch (p2->type) {
                case INTEGER:
                    return Object_initInteger(p1->data.Integer.v - p2->data.Integer.v);
                case DOUBLE_:
                    return Object_initDouble(p1->data.Integer.v - p2->data.Double.v);
                case RATIO:
                    return sub_rat(p1->data.Integer.v, 1,
                                   p2->data.Ratio.numer, p2->data.Ratio.denom);
                default:
                    printf("ERROR: - invalid data type");
                    return GLOBAL_NULL;
            }
            break;
        case DOUBLE_:
            switch (p2->type) {
                case INTEGER:
                    return Object_initDouble(p1->data.Double.v - p2->data.Integer.v);
                case DOUBLE_:
                    return Object_initDouble(p1->data.Double.v - p2->data.Double.v);
                case RATIO:
                    return Object_initDouble(p1->data.Double.v - p2->data.Ratio.numer/p2->data.Ratio.denom);
                default:
                    printf("ERROR: - invalid data type");
                    return GLOBAL_NULL;;
            }
            break;
        case RATIO:
            switch (p2->type) {
                case INTEGER:
                    return sub_rat(p1->data.Ratio.numer, p1->data.Ratio.denom, p2->data.Integer.v, 1);
                case DOUBLE_:
                    return Object_initDouble(p1->data.Ratio.numer/p1->data.Ratio.denom - p2->data.Double.v);
                case RATIO:
                    return sub_rat(p1->data.Ratio.numer, p1->data.Ratio.denom, p2->data.Ratio.numer, p2->data.Ratio.denom);
                default:
                    printf("ERROR: - invalid data type");
                    return GLOBAL_NULL;
            }
            break;
        default:
            printf("ERROR: - invalid data type");
            return GLOBAL_NULL;
    }
}
// 4 -
Object *builtin_sub(Object ** params, uint32_t param_num){
    Object * return_val = params[0];
    Object *p1, * p2;
    int32_t i;
    if (param_num == 1) { // check (- x) like expression
        switch (return_val->type) {
            case INTEGER:
                return Object_initInteger(-return_val->data.Integer.v);
            case DOUBLE_:
                return Object_initDouble(-return_val->data.Double.v);
            case RATIO:
                return Object_initRatio(-return_val->data.Ratio.numer, return_val->data.Ratio.denom);
            default:
                printf("ERROR: - invalid data type");
                return GLOBAL_NULL;
        }
    }
    for (i = 1; i < param_num; i++) {
        p2 = params[i];
        p1 = return_val;
        return_val = sub_2(return_val, p2);
        Object_free(p1);
    }
    return return_val;
}
Object * mul_2(Object * p1, Object * p2){
    switch (p1->type) {
        case INTEGER:
            switch (p2->type) {
                case INTEGER:
                    return Object_initInteger(p1->data.Integer.v * p2->data.Integer.v);
                case DOUBLE_:
                    return Object_initDouble(p1->data.Integer.v * p2->data.Double.v);
                case RATIO:
                    return mul_rat(p1->data.Integer.v, 1,
                                   p2->data.Ratio.numer, p2->data.Ratio.denom);
                default:
                    printf("ERROR: * invalid data type");
                    return GLOBAL_NULL;
            }
            break;
        case DOUBLE_:
            switch (p2->type) {
                case INTEGER:
                    return Object_initDouble(p1->data.Double.v * p2->data.Integer.v);
                case DOUBLE_:
                    return Object_initDouble(p1->data.Double.v * p2->data.Double.v);
                case RATIO:
                    return Object_initDouble(p1->data.Double.v * p2->data.Ratio.numer/p2->data.Ratio.denom);
                default:
                    printf("ERROR: * invalid data type");
                    return GLOBAL_NULL;;
            }
            break;
        case RATIO:
            switch (p2->type) {
                case INTEGER:
                    return mul_rat(p1->data.Ratio.numer, p1->data.Ratio.denom, p2->data.Integer.v, 1);
                case DOUBLE_:
                    return Object_initDouble(p1->data.Ratio.numer/p1->data.Ratio.denom * p2->data.Double.v);
                case RATIO:
                    return mul_rat(p1->data.Ratio.numer, p1->data.Ratio.denom, p2->data.Ratio.numer, p2->data.Ratio.denom);
                default:
                    printf("ERROR: * invalid data type");
                    return GLOBAL_NULL;
            }
            break;
        default:
            printf("ERROR: * invalid data type");
            return GLOBAL_NULL;
    }
}
// 5 *
Object *builtin_mul(Object ** params, uint32_t param_num){
    Object * return_val = params[0];
    Object *p1, * p2;
    uint32_t i;
    for (i = 1; i < param_num; i++) {
        p2 = params[i];
        p1 = return_val;
        return_val = mul_2(return_val, p2);
        Object_free(p1);
    }
    return return_val;

}
Object * div_2(Object * p1, Object * p2){
    switch (p1->type) {
        case INTEGER:
            switch (p2->type) {
                case INTEGER:
                    return div_rat(p1->data.Integer.v, 1, p2->data.Integer.v, 1); // only this one use div_rat
                case DOUBLE_:
                    return Object_initDouble(p1->data.Integer.v / p2->data.Double.v);
                case RATIO:
                    return div_rat(p1->data.Integer.v, 1,
                                   p2->data.Ratio.numer, p2->data.Ratio.denom);
                default:
                    printf("ERROR: / invalid data type");
                    return GLOBAL_NULL;
            }
            break;
        case DOUBLE_:
            switch (p2->type) {
                case INTEGER:
                    return Object_initDouble(p1->data.Double.v / p2->data.Integer.v);
                case DOUBLE_:
                    return Object_initDouble(p1->data.Double.v / p2->data.Double.v);
                case RATIO:
                    return Object_initDouble(p1->data.Double.v / p2->data.Ratio.numer/p2->data.Ratio.denom);
                default:
                    printf("ERROR: / invalid data type");
                    return GLOBAL_NULL;;
            }
            break;
        case RATIO:
            switch (p2->type) {
                case INTEGER:
                    return div_rat(p1->data.Ratio.numer, p1->data.Ratio.denom, p2->data.Integer.v, 1);
                case DOUBLE_:
                    return Object_initDouble(p1->data.Ratio.numer/p1->data.Ratio.denom / p2->data.Double.v);
                case RATIO:
                    return div_rat(p1->data.Ratio.numer, p1->data.Ratio.denom, p2->data.Ratio.numer, p2->data.Ratio.denom);
                default:
                    printf("ERROR: + invalid data type");
                    return GLOBAL_NULL;
            }
            break;
        default:
            printf("ERROR: + invalid data type");
            return GLOBAL_NULL;
    }
}
// 6 /
Object *builtin_div(Object ** params, uint32_t param_num){
    Object * return_val = params[0];
    Object *p1, * p2;
    int32_t i;
    for (i = 1; i < param_num; i++) {
        p2 = params[i];
        p1 = return_val;
        return_val = div_2(return_val, p2);
        Object_free(p1);
    }
    return return_val;


}
// 7 vector!
Object *builtin_vector(Object ** params, uint32_t param_num){
    int32_t size = (param_num / 16 + 1) * 16; // determine the size
    int32_t i = 0;
    Object * v = Object_initVector(1, size);
    Object *temp;
    for(; i < param_num; i++){
        temp = params[i];
        vector_Get(v, i) = temp;
        // increase temp use_count
        temp->use_count++;
    }
    v->data.Vector.length = param_num;
    return v;
}
// 8 vector
Object *builtin_vector_with_unchangable_length(Object ** params, uint32_t param_num){
    uint32_t i = 0;
    Object * v = Object_initVector(0, param_num);
    Object * temp;
    for(; i < param_num; i++){
        temp = params[i];
        vector_Get(v, i) = temp;
        temp->use_count++;
    }
    v->data.Vector.length = param_num;
    return v;
}
// 9 vector-length
Object *builtin_vector_length(Object ** params, uint32_t param_num){
    return Object_initInteger(vector_Length(params[0]));
}
// 10 vector-push!
Object *builtin_vector_push(Object ** params, uint32_t param_num){
    int64_t i = 0;
    Object * vec = params[0];
    Object * temp;
    uint64_t length = vector_Length(vec);
    uint64_t size = vector_Size(vec);
    for(i = 1; i < param_num; i++){
        if(length == size){
            if(vec->data.Vector.resizable){
                vec->data.Vector.v = (Object**)realloc(vec->data.Vector.v, sizeof(Object*) * (size * 2)); // increase size
                vec->data.Vector.size *= 2;
            }
            else{
                printf("ERROR: Cannot change size of vector\n");
                return GLOBAL_NULL;
            }
        }
        temp = params[i];
        vector_Set(vec, length, temp);
        temp->use_count++; // increase use count
        length++;
    }
    vec->data.Vector.length = length;
    return params[0];
}
// 11 vector-pop!
Object *builtin_vector_pop(Object ** params, uint32_t param_num){
    Object * vec = params[0];
    uint64_t length = vector_Length(vec);
    
    Object *return_out = vector_Get(vec, length - 1); // get pop value
    return_out->use_count--;                          // decrement use count
    
    vector_Get(vec, length - 1) = GLOBAL_NULL; // clear that variable
    length = length - 1;
    vec->data.Vector.length = length; // reset length
    return return_out;
}
int32_t num_equal_2(Object * p1, Object * p2){
    switch (p1->type) {
        case INTEGER:
            switch (p2->type) {
                case INTEGER:
                    return (p1->data.Integer.v == p2->data.Integer.v) ? 1 : 0;
                case DOUBLE_:
                    return (p1->data.Integer.v == p2->data.Double.v) ? 1 : 0;
                case RATIO:
                    return (p1->data.Integer.v == p2->data.Ratio.numer/p2->data.Ratio.denom) ? 1 : 0;
                default:
                    printf("ERROR: = invalid data type\n");
                    printf("     :%s\n", to_string(p2));
                    return 0;
            }
            break;
        case DOUBLE_:
            switch (p2->type) {
                case INTEGER:
                    return (p1->data.Double.v == p2->data.Integer.v) ?1 : 0;
                case DOUBLE_:
                    return (p1->data.Double.v == p2->data.Double.v) ? 1 : 0;
                case RATIO:
                    return (p1->data.Double.v == p2->data.Ratio.numer/p2->data.Ratio.denom) ? 1 : 0;
                default:
                    printf("ERROR: = invalid data type\n");
                    printf("     :%s\n", to_string(p2));
                    return 0;;
            }
            break;
        case RATIO:
            switch (p2->type) {
                case INTEGER:
                    return (p1->data.Ratio.numer/p1->data.Ratio.denom == p2->data.Integer.v) ? 1 : 0;
                case DOUBLE_:
                    return Object_initDouble(p1->data.Ratio.numer/p1->data.Ratio.denom == p2->data.Double.v)? 1 : 0;
                case RATIO:
                    return (p1->data.Ratio.numer/p1->data.Ratio.denom == p2->data.Ratio.numer/p2->data.Ratio.denom)? 1 : 0;
                default:
                    printf("ERROR: = invalid data type\n");
                    printf("     :%s\n", to_string(p2));
                    return 0;
            }
            break;
        default:
            printf("ERROR: = invalid data type\n");
            printf("     :%s\n", to_string(p1));
            return 0;
    }
}
// 12 =
Object *builtin_num_equal(Object ** params, uint32_t param_num){
    uint32_t i;
    Object *p1, *p2;
    for (i = 0; i < param_num - 1; i++) {
        p1 = params[i];
        p2 = params[i + 1];
        if (!num_equal_2(p1, p2)) {
            return GLOBAL_NULL;
        }
    }
    return GLOBAL_TRUE;
}
// 13 <
int32_t lt_2(Object * p1, Object * p2){
    switch (p1->type) {
        case INTEGER:
            switch (p2->type) {
                case INTEGER:
                    return (p1->data.Integer.v < p2->data.Integer.v) ? 1 : 0;
                case DOUBLE_:
                    return (p1->data.Integer.v < p2->data.Double.v) ? 1 : 0;
                case RATIO:
                    return (p1->data.Integer.v < p2->data.Ratio.numer/p2->data.Ratio.denom) ? 1 : 0;
                default:
                    printf("ERROR: < invalid data type\n");
                    printf("     :%s\n", to_string(p2));
                    return 0;
            }
            break;
        case DOUBLE_:
            switch (p2->type) {
                case INTEGER:
                    return (p1->data.Double.v < p2->data.Integer.v) ?1 : 0;
                case DOUBLE_:
                    return (p1->data.Double.v < p2->data.Double.v) ? 1 : 0;
                case RATIO:
                    return (p1->data.Double.v < p2->data.Ratio.numer/p2->data.Ratio.denom) ? 1 : 0;
                    
                default:
                    printf("ERROR: < invalid data type\n");
                    printf("     :%s\n", to_string(p2));
                    return 0;;
            }
            break;
        case RATIO:
            switch (p2->type) {
                case INTEGER:
                    return (p1->data.Ratio.numer/p1->data.Ratio.denom < p2->data.Integer.v) ? 1 : 0;
                case DOUBLE_:
                    return Object_initDouble(p1->data.Ratio.numer/p1->data.Ratio.denom < p2->data.Double.v)? 1 : 0;
                case RATIO:
                    return (p1->data.Ratio.numer/p1->data.Ratio.denom < p2->data.Ratio.numer/p2->data.Ratio.denom)? 1 : 0;
                default:
                    printf("ERROR: < invalid data type\n");
                    printf("     :%s\n", to_string(p2));
                    return 0;
            }
            break;
        default:
            return (uint64_t)p1 < (uint64_t)p2 ? 1 : 0;
            //printf("ERROR: < invalid data type\n");
            //printf("     :%s\n", to_string(p1));
            //return 0;
    }
}
Object *builtin_num_lt(Object ** params, uint32_t param_num){
    uint32_t i;
    Object *p1, *p2;
    for (i = 0; i < param_num - 1; i++) {
        p1 = params[i];
        p2 = params[i + 1];
        if (!lt_2(p1, p2)) {
            return GLOBAL_NULL;
        }
    }
    return GLOBAL_TRUE;
}

int32_t le_2(Object * p1, Object * p2){
    switch (p1->type) {
        case INTEGER:
            switch (p2->type) {
                case INTEGER:
                    return (p1->data.Integer.v <= p2->data.Integer.v) ? 1 : 0;
                case DOUBLE_:
                    return (p1->data.Integer.v <= p2->data.Double.v) ? 1 : 0;
                case RATIO:
                    return (p1->data.Integer.v <= p2->data.Ratio.numer/p2->data.Ratio.denom) ? 1 : 0;
                default:
                    printf("ERROR: <= invalid data type\n");
                    printf("     :%s\n", to_string(p2));
                    return 0;
            }
            break;
        case DOUBLE_:
            switch (p2->type) {
                case INTEGER:
                    return (p1->data.Double.v <= p2->data.Integer.v) ?1 : 0;
                case DOUBLE_:
                    return (p1->data.Double.v <= p2->data.Double.v) ? 1 : 0;
                case RATIO:
                    return (p1->data.Double.v <= p2->data.Ratio.numer/p2->data.Ratio.denom) ? 1 : 0;
                    
                default:
                    printf("ERROR: <= invalid data type\n");
                    printf("     :%s\n", to_string(p2));
                    return 0;;
            }
            break;
        case RATIO:
            switch (p2->type) {
                case INTEGER:
                    return (p1->data.Ratio.numer/p1->data.Ratio.denom <= p2->data.Integer.v) ? 1 : 0;
                case DOUBLE_:
                    return Object_initDouble(p1->data.Ratio.numer/p1->data.Ratio.denom <= p2->data.Double.v)? 1 : 0;
                case RATIO:
                    return (p1->data.Ratio.numer/p1->data.Ratio.denom <= p2->data.Ratio.numer/p2->data.Ratio.denom)? 1 : 0;
                default:
                    printf("ERROR: <= invalid data type\n");
                    printf("     :%s\n", to_string(p2));
                    return 0;
            }
            break;
        default:
            return (uint64_t)p1 <= (uint64_t)p2 ? 1 : 0;
            //printf("ERROR: <= invalid data type\n");
            //printf("     :%s\n", to_string(p1));
            //return 0;
    }

}
// 14 <=
Object *builtin_num_le(Object ** params, uint32_t param_num){
    uint32_t i;
    Object *p1, *p2;
    for (i = 0; i < param_num - 1; i++) {
        p1 = params[i];
        p2 = params[i + 1];
        if (!le_2(p1, p2)) {
            return GLOBAL_NULL;
        }
    }
    return GLOBAL_TRUE;
}
int32_t eq_2(Object * p1, Object * p2){
    if(( p1->type == INTEGER || p1->type == DOUBLE_ || p1->type == RATIO) && (p2->type == INTEGER || p2->type == DOUBLE_ || p2->type == RATIO)){
        return num_equal_2(p1, p2);
    }
    return (p1 == p2) ? 1 : 0;
}
// 15 eq?
Object *builtin_eq(Object ** params, uint32_t param_num){
    uint32_t i;
    for (i = 0; i < param_num - 1; i++) {
        Object * p1 = params[i];
        Object * p2 = params[i + 1];
        if (!eq_2(p1, p2)) {
            return GLOBAL_NULL;
        }
    }
    return GLOBAL_TRUE;
}
/*
    16 ~ 21 will be changed in the future.
    because I am now using "typeof" function to determine the data type
 */
/*
// 16 string?
Object *builtin_string_type(Object ** params, int param_num){
    if(params[0]->type == STRING)
        return GLOBAL_TRUE;
    return GLOBAL_NULL;
}
 
 16 ===> load int_index 1
 */
/*
 17 exit
 eg: (exit 0)  print 0 then exit the program
 */
Object * builtin_exit(Object ** params, uint32_t param_num){
    if (param_num == 0) {
        printf("0\n");
        exit(0);
    }
    printf("%s\n", to_string(params[0]));
    exit((int)params[0]->data.Integer.v);
}
/*
   18 >
 */
Object *builtin_gt(Object ** params, uint32_t param_num){
    int32_t i;
    for (i = param_num - 2; i >= 0; i--) {
        if (le_2(params[i], params[i + 1])) {
            return GLOBAL_NULL;
        }
    }
    return GLOBAL_TRUE;
}
/*
   19 >=
 */
Object *builtin_ge(Object ** params, uint32_t param_num){
    int32_t i;
    for (i = param_num - 2; i >= 0; i--) {
        if (lt_2(params[i], params[i + 1])) {
            return GLOBAL_NULL;
        }
    }
    return GLOBAL_TRUE;
}
// 20 parse
// (parse "(def x 12)") => ((def x 12))
Object *builtin_parse(Object ** params, uint32_t param_num){
    Lexer * l = lexer(params[0]->data.String.v);
    if (l == NULL) {
        printf("ERROR: parentheses () num doesn't match");
    }
    return parser(l);
}
// 21 random
Object *builtin_random(Object ** params, uint32_t param_num){
    double r = rand();
    return Object_initDouble(r/RAND_MAX);
}
// 22 strcmp  compare string
Object *builtin_strcmp(Object ** params, uint32_t param_num){
    int32_t i = strcmp(params[0]->data.String.v, params[1]->data.String.v);
    return Object_initInteger(i);
}
// 23 string-slice
Object *builtin_string_slice(Object ** params, uint32_t param_num){
    char * s = params[0]->data.String.v;
    int64_t start = params[1]->data.Integer.v;
    int64_t end = params[2]->data.Integer.v;
    int64_t length = end - start;
    char * out = (char*)malloc(sizeof(char) * (length + 1));
    uint32_t i = 0;
    for(; i < length; i++){
        out[i] = s[i + start];
    }
    out[i] = 0;
    return Object_initString(out, length);
}
// 24 string-length
Object *builtin_string_length(Object ** params, uint32_t param_num){
    return Object_initInteger(string_Length(params[0]));
}
// 25 string-append
Object *builtin_string_append(Object ** params, uint32_t param_num){
    Object * s1 = params[0];
    Object * s2 = params[1];
    uint64_t sum_length = string_Length(s1) + string_Length(s2) + 1;
    char *out_ = (char*)malloc(sizeof(char)*(sum_length));
    strcpy(out_, s1->data.String.v);
    strcat(out_, s2->data.String.v);
    //out[sum_length] = 0;
    s1 = Object_initString(out_, sum_length - 1);
    free(out_);
    return s1;
}
// 26 table
// (table 'a 12 'b 16)
Object *builtin_make_table(Object ** params, uint32_t param_num){
    Object * table = Object_initTable( param_num == 0? 16 : (int)param_num/0.6);
    uint32_t i = 0;
    for(; i < param_num; i = i + 2){
        Table_setval(table, params[i], params[i + 1]);
    }
    return table;
}
// 27 table-keys
Object *builtin_table_keys(Object ** params, uint32_t param_num){
    Object * table = params[0];
    return table_getKeys(table);
}
// 28 table-delete
Object *builtin_table_delete(Object ** params, uint32_t param_num){
    Object * table = params[0];
    Object * key = params[1];
    uint64_t hash_value = hash(key->data.String.v, table->data.Table.size);
    Table_Pair * table_pairs = table->data.Table.vec[hash_value]; // get pairs
    while(table_pairs!=NULL){
        if( table_pairs->key == key || strcmp(key->data.String.v, table_pairs->key->data.String.v) == 0){
            table_pairs->value->use_count--;
            Object_free(table_pairs->value);
            table_pairs->value = GLOBAL_NULL;
            return GLOBAL_TRUE;
        }
        table_pairs = table_pairs->next;
    }
    // didnt find
    return GLOBAL_NULL;
}
// 29 file-read
// (file-read "test.toy")
// return string
Object * builtin_file_read(Object ** params, uint32_t param_num){
    char * file_name = params[0]->data.String.v;
    FILE* file = fopen(file_name,"r");
    if(file == NULL)
    {
        return GLOBAL_NULL; // fail to read
    }
    
    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);
    
    char* content = (char*)calloc(size + 1, 1);
    
    fread(content,1,size,file);
    
    fclose(file); // 不知道要不要加上这个
    return Object_initString(content, size);
}
// 30 file-write
// (file-write "test.toy", "Hello World")
// return null
Object * builtin_file_write(Object ** params, uint32_t param_num){
    char * file_name = params[0]->data.String.v;
    FILE* file = fopen(file_name,"w");
    fputs(params[1]->data.String.v, file);
    fclose(file);
    return GLOBAL_NULL;
}
// 31 sys-argv
// 32 int->string
// (int->string 12) => "12"
// (int->string 12 "%x") => "0x3"
Object * builtin_int_to_string(Object ** params, uint32_t param_num){
    char *d; // format
    char b[100];
    char * o;
    switch(param_num){
        case 1:
            sprintf(b, "%lld", (long long int)params[0]->data.Integer.v);
            o = (char*)malloc(sizeof(char) * (strlen(b) + 1));
            strcpy(o, b);
            return Object_initString(o, strlen(o));
        case 2:
            d = params[1]->data.String.v;
            sprintf(b, d, params[0]->data.Integer.v);
            o = (char*)malloc(sizeof(char) * (strlen(b) + 1));
            strcpy(o, b);
            return Object_initString(o, strlen(o));
        default:
            printf("ERROR: int->string invalid args\n");
            return GLOBAL_NULL;
    }
}

// 33 floag->string
Object * builtin_float_to_string(Object ** params, uint32_t param_num){
    char *d; // format
    char b[100];
    char * o;
    switch(param_num){
        case 1:
            sprintf(b, "%.20f", params[0]->data.Double.v);
            o = (char*)malloc(sizeof(char) * (strlen(b) + 1));
            strcpy(o, b);
            return Object_initString(o, strlen(o));
        case 2:
            d = params[1]->data.String.v;
            sprintf(b, d, params[0]->data.Double.v);
            o = (char*)malloc(sizeof(char) * (strlen(b) + 1));
            strcpy(o, b);
            return Object_initString(o, strlen(o));
        default:
            printf("ERROR: float->string invalid args\n");
            return GLOBAL_NULL;
    }
}
// 34 input
// (def x (input)) return string
Object * builtin_input(Object ** params, uint32_t param_num){
    if(param_num == 1){
        printf("%s", params[0]->data.String.v);
    }
    char * buffer;
    size_t n;
    ssize_t read_n = getline(&buffer, &n, stdin);
    if (read_n > 0 && buffer[read_n - 1] == '\n' ) {
        buffer[read_n - 1] = '\0';
        read_n--;
    }
    //static char buffer[1024]; // so wont be free again
    //fgets(buffer, 1024, stdin)
    ;
    Object * o = allocateObject();
    o->type = STRING;
    o->data.String.v = buffer;
    o->data.String.length = read_n;
    return o;
}
// 35 display  原先是 display-string
Object * builtin_display_string(Object ** params, uint32_t param_num){
    if (COMPILATION_MODE) { // under compilation mode, no print
        return GLOBAL_NULL;
    }
    uint32_t i = 0;
    char * s;
    while ( i < param_num) {
        s = to_string(params[i]);
        printf("%s", s);
        //fputs(s, stdout);
        free(s);
        i++;
    }
    return GLOBAL_NULL;
}
// 36 string->int
Object * builtin_string_to_int(Object ** params, uint32_t param_num){
    // 以后用atol
    char * v = params[0]->data.String.v;
    char * t = NULL;
    if (strlen(v) >= 3 && v[0] == '0' && v[1] == 'x') { // hex
        return Object_initInteger(strtoull(v, &t, 16));
    }
    else
        return Object_initInteger(strtoull(v, &t, 10));
}
// 37 string->float
Object * builtin_string_to_float(Object ** params, uint32_t param_num){
    return Object_initDouble(atof(params[0]->data.String.v));
}
// 因为要经常用到 null? 函数
// 38 null?
Object * builtin_null_type(Object ** params, uint32_t param_num){
    return params[0] == GLOBAL_NULL ? GLOBAL_TRUE : GLOBAL_NULL;
}
// 39 numer
Object * builtin_numer(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER: case DOUBLE_:
            return p;
        case RATIO:
            return Object_initInteger(p->data.Ratio.numer);
        default:
            printf("ERROR: Function numer invalid parameter");
            return GLOBAL_NULL;
    }
}
// 40 denom
Object * builtin_denom(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER: case DOUBLE_:
            return Object_initInteger(1);
        case RATIO:
            return Object_initInteger(p->data.Ratio.denom);
        default:
            printf("ERROR: Function denom invalid parameter");
            return GLOBAL_NULL;
    }
}

// 41 gensym
Object * builtin_gensym(Object ** params, uint32_t param_num){
    char buffer[48];
    sprintf(buffer, "__toy__%d", GEN_SYM_COUNT);
    GEN_SYM_COUNT+=6; // I like 6
    return Object_initString(buffer, strlen(buffer));
}

// 42 add-proto
Object * builtin_table_add_proto(Object ** params, uint32_t param_num){
    params[0]->data.Table.proto = params[1]; // set proto
    params[1]->use_count++;
    params[0]->type = OBJECT; // change type
    return GLOBAL_NULL;
}

// 43 proto
Object * builtin_table_proto(Object ** params, uint32_t param_num){
    return params[0]->data.Table.proto;
}

// 44 typeof
Object * builtin_typeof(Object ** params, uint32_t param_num){
    switch (params[0]->type) {
        case INTEGER:
            return INTEGER_STRING;
        case DOUBLE_:
            return FLOAT_STRING;
        case RATIO:
            return RATIO_STRING;
        case STRING:
            return STRING_STRING;
        case PAIR:
            return PAIR_STRING;
        case USER_DEFINED_LAMBDA:case BUILTIN_LAMBDA:
            return LAMBDA_STRING;
        case VECTOR:
            return VECTOR_STRING;
        case TABLE:
            return TABLE_STRING;
        case OBJECT:
            return STRING_object;
        default:
            return GLOBAL_NULL;
    }
}


// 45 math-cos
Object * builtin_cos(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
            return Object_initDouble(cos((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(cos(p->data.Double.v));
        case RATIO:
            return Object_initDouble(cos((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-cos invalid param type");
            return GLOBAL_NULL;
    }
}
// 46 math-sin
Object * builtin_sin(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(sin((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(sin(p->data.Double.v));
        case RATIO:
            return Object_initDouble(sin((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-sin invalid param type");
            return GLOBAL_NULL;
    }
}
// 47 math-tan
Object * builtin_tan(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(tan((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(tan(p->data.Double.v));
        case RATIO:
            return Object_initDouble(tan((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-tan invalid param type");
            return GLOBAL_NULL;
    }
}
// 48 math-acos
Object * builtin_acos(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(acos((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(acos(p->data.Double.v));
        case RATIO:
            return Object_initDouble(acos((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-acos invalid param type");
            return GLOBAL_NULL;
    }
}
// 49 math-asin
Object * builtin_asin(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(asin((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(asin(p->data.Double.v));
        case RATIO:
            return Object_initDouble(asin((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-asin invalid param type");
            return GLOBAL_NULL;
    }
}
// 50 math-atan
Object * builtin_atan(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(atan((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(atan(p->data.Double.v));
        case RATIO:
            return Object_initDouble(atan((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-atan invalid param type");
            return GLOBAL_NULL;
    }
}
// 51 math-cosh
Object * builtin_cosh(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(cosh((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(cosh(p->data.Double.v));
        case RATIO:
            return Object_initDouble(cosh((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-cosh invalid param type");
            return GLOBAL_NULL;
    }
}
// 52 math-sinh
Object * builtin_sinh(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(sinh((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(sinh(p->data.Double.v));
        case RATIO:
            return Object_initDouble(sinh((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-sinh invalid param type");
            return GLOBAL_NULL;
    }
}
// 53 math-tanh
Object * builtin_tanh(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(tanh((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(tanh(p->data.Double.v));
        case RATIO:
            return Object_initDouble(tanh((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-tanh invalid param type");
            return GLOBAL_NULL;
    }
}

// 54 math-log
Object * builtin_log(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(log((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(log(p->data.Double.v));
        case RATIO:
            return Object_initDouble(log((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-log invalid param type");
            return GLOBAL_NULL;
    }
}
// 55 math-exp
Object * builtin_exp(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(exp((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(exp(p->data.Double.v));
        case RATIO:
            return Object_initDouble(exp((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-exp invalid param type");
            return GLOBAL_NULL;
    }
}
// 56 math-log10
Object * builtin_log10(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(log10((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(log10(p->data.Double.v));
        case RATIO:
            return Object_initDouble(log10((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-log10 invalid param type");
            return GLOBAL_NULL;
    }
}
// 57 math-pow
Object * builtin_pow(Object ** params, uint32_t param_num){
    Object * p1 = params[0];
    Object * p2 = params[1];
    switch (p1->type) {
        case INTEGER:
            switch (p2->type) {
                case INTEGER:
					return Object_initInteger(pow((double)p1->data.Integer.v, (double)p2->data.Integer.v));
                case DOUBLE_:
                    return Object_initDouble(pow(p1->data.Integer.v, p2->data.Double.v));
                case RATIO:
                    return Object_initDouble(pow(p1->data.Integer.v, p2->data.Ratio.numer / (double)p2->data.Ratio.denom));
                default:
                    printf("ERROR: Function math-pow invalid param type");
                    return GLOBAL_NULL;
            }
        case DOUBLE_:
            switch (p2->type) {
                case INTEGER:
					return Object_initDouble(pow((double)p1->data.Double.v, (double)p2->data.Integer.v));
                case DOUBLE_:
                    return Object_initDouble(pow(p1->data.Double.v, p2->data.Double.v));
                case RATIO:
                    return Object_initDouble(pow(p1->data.Double.v, p2->data.Ratio.numer / (double)p2->data.Ratio.denom));
                default:
                    printf("ERROR: Function math-pow invalid param type");
                    return GLOBAL_NULL;
            }
        case RATIO:
            switch (p2->type) {
                case INTEGER:
					return Object_initRatio(pow((double)p1->data.Ratio.numer, (double)p2->data.Integer.v),
											pow((double)p1->data.Ratio.denom, (double)p2->data.Integer.v));
                    break;
                case DOUBLE_:
                    return Object_initDouble(pow((double)p1->data.Ratio.numer / p1->data.Ratio.denom, p2->data.Double.v));
                case RATIO:
                    return Object_initDouble(pow((double)p1->data.Ratio.numer / p1->data.Ratio.denom,
                                                 (double)p2->data.Ratio.numer / p2->data.Ratio.denom));
                default:
                    printf("ERROR: Function math-pow invalid param type");
                    return GLOBAL_NULL;
            }

        default:
            printf("ERROR: Function math-pow invalid param type");
            return GLOBAL_NULL;
    }
}
// 58 math-sqrt
Object * builtin_sqrt(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(sqrt((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(sqrt(p->data.Double.v));
        case RATIO:
            return Object_initDouble(sqrt((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-sqrt invalid param type");
            return GLOBAL_NULL;
    }
}
// 59 math-ceil
Object * builtin_ceil(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(ceil((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(ceil(p->data.Double.v));
        case RATIO:
            return Object_initDouble(ceil((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-ceil invalid param type");
            return GLOBAL_NULL;
    }
}
// 60 math-floor
Object * builtin_floor(Object ** params, uint32_t param_num){
    Object * p = params[0];
    switch (p->type) {
        case INTEGER:
			return Object_initDouble(floor((double)p->data.Integer.v));
        case DOUBLE_:
            return Object_initDouble(floor(p->data.Double.v));
        case RATIO:
            return Object_initDouble(floor((double)p->data.Ratio.numer / p->data.Ratio.denom));
        default:
            printf("ERROR: Function math-floor invalid param type");
            return GLOBAL_NULL;
    }
}

// 61 string-find
// (string-find "Hello" "H")
// (string-find "Hello" "l" 3) return find index
// if not found, return (), I prefer return -1..
Object * builtin_string_find(Object ** params, uint32_t param_num){
    Object * s = params[0];
    Object * find_s = params[1];
    char * p;
    if (param_num == 2) {
        p = strstr(s->data.String.v, find_s->data.String.v);
    str_test:
        if (p) {
            return Object_initInteger(p - s->data.String.v);
        }
        else{
            return GLOBAL_NULL;
        }
    }
    else{
        p = strstr(s->data.String.v + params[2]->data.Integer.v,
                   find_s->data.String.v);
        goto str_test;
    }
}

// 62 string-replace
// (string-find "Hello" "H" "a")
// (string-find "Hello" "l" "a" 3)
Object * builtin_string_replace(Object ** params, uint32_t param_num){
    Object * s = params[0];
    Object * find_s = params[1];
    Object * replace_s = params[2];
    char * p;
    char buffer[4096] = "";
    if (param_num == 3) {
        p = strstr(s->data.String.v, find_s->data.String.v);
    str_test:
        if (p) {
            strncat(buffer, s->data.String.v, p - s->data.String.v); // copy
            strcat(buffer, replace_s->data.String.v);
            strcat(buffer, p + find_s->data.String.length);
            return Object_initString(buffer, strlen(buffer));
        }
        else{
            return s; // didn't replace
        }
    }
    else{
        p = strstr(s->data.String.v + params[3]->data.Integer.v,
                   find_s->data.String.v);
        goto str_test;
    }
}
// 63 apply
// 64 vector-slice
Object * builtin_vector_slice(Object ** params, uint32_t param_num){
    Object * p = params[0];
    // only support integer param
    uint64_t start = params[1]->data.Integer.v;
    uint64_t end;
    switch (param_num) {
        case 2:
            end = p->data.Vector.length;
            break;
        case 3:
            end = params[2]->data.Integer.v;
            break;
        default:
            printf("ERROR: Function vector-slice invalid param num\n");
            return GLOBAL_NULL;
            break;
    }
    Object * o = Object_initVector(1, p->data.Vector.size);
    Object * t;
    uint64_t i;
    for (i = start; i < end; i++){
        t = p->data.Vector.v[i];
        t->use_count++;
        o->data.Vector.v[i - start] = t;
    }
    o->data.Vector.length = end - start;
    return o;
}
// 65 set-car! (set-car! x 3)
Object * builtin_set_car(Object ** params, uint32_t param_num){
    Object * p1 = params[0];
    Object * p2 = params[1];
    p1->data.Pair.car->use_count--;
    Object_free(p1->data.Pair.car);
    p1->data.Pair.car = p2;
    p2->use_count++;
    return p1;
}
// 66 set-cdr! (set-cdr! x 3)
Object * builtin_set_cdr(Object ** params, uint32_t param_num){
    Object * p1 = params[0];
    Object * p2 = params[1];
    p1->data.Pair.cdr->use_count--;
    Object_free(p1->data.Pair.cdr);
    p1->data.Pair.cdr = p2;
    p2->use_count++;
    return p1;
}

// 67 system
Object * builtin_system(Object ** params, uint32_t param_num){
    system(params[0]->data.String.v);
    return GLOBAL_NULL;
}

// 68 <<
Object * builtin_left_shift(Object ** params, uint32_t param_num){
    return Object_initInteger(params[0]->data.Integer.v << params[1]->data.Integer.v);
}
// 69 >>  signed right shift
Object * builtin_right_shift(Object ** params, uint32_t param_num){
    return Object_initInteger(params[0]->data.Integer.v >> params[1]->data.Integer.v);
}
// 70 >>> unsigned right shift
Object * builtin_unsigned_right_shift(Object ** params, uint32_t param_num){
    return Object_initInteger((uint64_t)params[0]->data.Integer.v >> params[1]->data.Integer.v);
}
// 71 & bitwise-and
Object * builtin_bitwise_and(Object ** params, uint32_t param_num){
    return Object_initInteger(params[0]->data.Integer.v & params[1]->data.Integer.v);
}
// 72 | bitwise-or
Object * builtin_bitwise_or(Object ** params, uint32_t param_num){
    return Object_initInteger(params[0]->data.Integer.v | params[1]->data.Integer.v);
}
// 73 abs_path
Object * builtin_abs_path(Object ** params, uint32_t param_num){
    char abs_path[256];
    // get absolute path
#ifdef WIN32
    GetFullPathName((TCHAR*)params[0]->data.String.v, 256, (TCHAR*)abs_path, NULL); // I don't know is this correct
#else
    realpath(params[0]->data.String.v, abs_path);
#endif
    return Object_initString(abs_path, strlen(abs_path));
}
// 74 float->int64
Object * builtin_float_to_int64(Object ** params, uint32_t param_num){
    uint64_t * unsigned_int_ = (uint64_t *)&(params[0]->data.Double.v); // get hex
    return Object_initInteger(*unsigned_int_);
}

//  75 table-length
Object * builtin_table_length(Object ** params, uint32_t param_num){
    return Object_initInteger(params[0]->data.Table.length);
}

//  76 string-char-code-at
// (string-char-code-at "hi" 0) => 104
Object * builtin_string_char_code_at(Object ** params, uint32_t param_num){
    return Object_initInteger(params[0]->data.String.v[params[1]->data.Integer.v]);
}

//  77 string-from-char-code
Object * builtin_string_from_char_code(Object ** params, uint32_t param_num){
    char * s = malloc(sizeof(char) * (param_num + 1));
    uint16_t i;
    Object * o;
    for (i = 0; i < param_num; i++) {
        s[i] = params[i]->data.Integer.v;
    }
    s[i] = 0;
    o = Object_initString(s, param_num);
    free(s);
    return o;
}

// 78 os-fork
// return pid
Object * builtin_os_fork(Object ** params, uint32_t param_num){
    pid_t pid = fork();
    return Object_initInteger(pid);
}

// 79 os-getpid
Object * builtin_os_getpid(Object ** params, uint32_t param_num){
    return Object_initInteger(getpid());
}

// 80 os-getppid
Object * builtin_os_getppid(Object ** params, uint32_t param_num){
    return Object_initInteger(getppid());
}

// 81 os-waitpid
// waitpid(pid, options) return status
Object * builtin_os_waitpid(Object ** params, uint32_t param_num){
    int status;
    if (param_num == 1) {
        waitpid((pid_t)params[0]->data.Integer.v, &status, 0);
    }
    else{
        waitpid((pid_t)params[0]->data.Integer.v, &status, (pid_t)params[1]->data.Integer.v);
    }
    return Object_initInteger(status);
}

// 82 os-getenv
Object * builtin_os_getenv(Object ** params, uint32_t param_num){
    char * val = getenv(params[0]->data.String.v);
    return Object_initString(val, strlen(val));
}

// 82 sleep
// (sleep sleep-seconds)
Object * builtin_sleep(Object ** params, uint32_t param_num){
    sleep((unsigned int)params[0]->data.Integer.v);
    return GLOBAL_NULL;
}

#endif
