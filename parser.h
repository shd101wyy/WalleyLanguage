/*
 parser.c
 */
#include "lexer.h"
#ifndef PARSER_C
#define PARSER_C

#define str_eq(a, b) (strcmp((a), (b)) == 0)
// get tag for ' ~ ~@
Object * parser_get_tag(char * s){
    // char * tag;
    if(strcmp(s, "'") == 0) return QUOTE_STRING;
    else if (strcmp(s, "~") == 0) return UNQUOTE_STRING;
    else if (strcmp(s, "~@") == 0) return UNQUOTE_SPLICE_STRING;
    else return QUASIQUOTE_STRING;
}
Object * formatQuickAccess(char**keys, int32_t n, int32_t count, Object * output){
    if(count == n) return output;
    return formatQuickAccess(keys,
                             n,
                             count+1,
                             cons(output, cons(cons(QUOTE_STRING,
                                                    cons(Object_initString(keys[count], strlen(keys[count])),
                                                         GLOBAL_NULL)),
                                               GLOBAL_NULL)));
    
}
int32_t isDouble (char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
        return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}
int32_t isInteger(char * s){
    if (s == NULL || *s == '\0' || isspace(*s))
        return 0;
    char * p;
    if (strlen(s) >= 3 && s[0] == '0' && s[1] == 'x') {
        strtol( &(*(s+2)), &p, 16); // hex
    }
    else
        strtol(s, &p, 10); // decimal
    return *p == '\0';
}
/*
    free parser
 */
void parser_free(Object * p){
    /*
     这个函数有问题， 以后用object_free
     */
    /*
    Object * v;
    Object * temp;
    while(p!=GLOBAL_NULL){
        v = car(p);
        switch (v->type) {
            case STRING:
                free(v->data.String.v);
                free(v);
                break;
            case INTEGER:case DOUBLE:
                free(v);
                break;
            case PAIR:
                parser_free(v);
                break;
            case NULL_:
                break;
            default:
                printf("## ERROR please report bug:\n    parser_free error.");
                break;
        }
        temp = p;
        p = cdr(p);
        free(temp);
        temp = NULL;
    }*/
    Object_free(p);
}
Object * parser(Lexer * le){
    if (le == NULL) { // lexer failure
        return GLOBAL_NULL;
    }
    char ** l = le->string_array;
    uint32_t length = le->array_length;
    Object * current_list_pointer = GLOBAL_NULL;
    int32_t i;
    uint32_t j, k, n, start;
    Object * lists = GLOBAL_NULL;//cons(GLOBAL_NULL, GLOBAL_NULL);
    Object * temp = NULL;
    char * splitted_[100]; // max 100 string
    char * t = NULL;
    char * ns;
    for(i = length - 1; i >= 0; i--){
        // printf("@ %s\n", l[i]);
        if(str_eq(l[i], ")")){
            lists = cons(current_list_pointer, lists); // save current lists
            current_list_pointer = GLOBAL_NULL;        // reset current_list_pointer
        }
        else if (str_eq(l[i], "(")){
            if(i!=0 &&
               (str_eq(l[i-1], "~@") || str_eq(l[i-1], "'") || str_eq(l[i-1], "~") || str_eq(l[i-1], "`"))){

                current_list_pointer = cons(cons(parser_get_tag(l[i-1]),
                                                   cons(current_list_pointer, GLOBAL_NULL)),
                                              car(lists));;
                i--;
            }
            else{
                current_list_pointer = cons(current_list_pointer, car(lists)); // append list
            }
            // need to free lists here
            // because that pointer is not used anymore
            temp = lists;
            cdr(lists)->use_count += 1; // has to add that, otherwise it will be freed when calling Object_free(temp);
            lists = cdr(lists);
            Object_free(temp);
            (lists)->use_count -= 1; // restore use count
/*
    lists    current_list_pointer
    (())           ()
    (() ())        ()
    (())          (())
     ()          ((()))
 */
        }
        else{
            temp = NULL;
            // check Math:add like (Math 'add)
            if(l[i][0] == '"' || l[i][0] == ':' || l[i][(uint32_t)strlen(l[i])-1] == ':'){
                if (l[i][0] == ':') { // :a  =>  "a"
                    t = malloc(sizeof(char) * strlen(l[i])+2); // " " 0
                    t[0] = '"';
                    for (j = 1; j < strlen(l[i]); j++) {
                        t[j] = l[i][j];
                    }
                    t[j] = '"';
                    t[j+1] = 0;
                    temp = Object_initString(t, j+1);
                    free(t);
                }
                else if(isInteger(l[i])){
                    if (strlen(l[i]) >= 3 && l[i][0] == '0' && l[i][1] == 'x') { // hex
                        temp = Object_initInteger(strtol(l[i], &t, 16));
                    }
                    else
                        temp = Object_initInteger(strtol(l[i], &t, 10));
                }
                else if(isDouble(l[i]))
                    temp = Object_initDouble(strtod(l[i], &t));
                else
                    temp = Object_initString(l[i], strlen(l[i]));
            }
            else{
                // split string
                n = 0; // splitted_length
                start = 0;
                for(j = 0; j < (uint32_t)strlen(l[i]); j++){
                    if(l[i][j] == ':'){
                        /*char */ t = (char*)malloc(sizeof(char)*(j - start + 1));
                        for(k = start; k < j; k++){
                            t[k-start] = l[i][k];
                        }
                        t[k-start] = 0;
                        start = j+1;
                        splitted_[n] = t;
                        n++; // increase size
                    }
                }
                // append last
                t = (char*)malloc(sizeof(char)*(j - start + 1));
                for(k = start; k < j; k++){
                    t[k-start] = l[i][k];
                }
                t[k-start] = 0;
                splitted_[n] = t;
                n++; // increase size
                
                ns = splitted_[0]; // get ns. eg x:a => ns 'x' keys ['a']
                if(n == 1){ // 没有找到 :
                    if(isInteger(l[i])){
                        if (strlen(l[i]) >= 3 && l[i][0] == '0' && l[i][1] == 'x') { // hex
                            temp = Object_initInteger(strtol(l[i], &t, 16));
                        }
                        else
                            temp = Object_initInteger(strtol(l[i], &t, 10));
                    }
                    else if(isDouble(l[i])){
                        temp = Object_initDouble(strtod(l[i], &t));
                    }
                    else
                        temp = Object_initString(l[i], strlen(l[i]));
                }
                else{
                    temp = formatQuickAccess(
                                             splitted_,
                                             n,
                                             2,
                                             cons(Object_initString(ns, strlen(ns)),
                                                  cons(cons(QUOTE_STRING,
                                                            cons(Object_initString(splitted_[1], strlen(splitted_[1])),
                                                                 GLOBAL_NULL)),
                                                       GLOBAL_NULL)));
                }
                // free splitted_
                // also t, which is the last element of splitted_
                // will be freed
                for (j = 0; j < n; j++) {
                    free(splitted_[j]);
                    splitted_[j] = NULL;
                }
            }
            
            if(i!=0 &&
               (str_eq(l[i-1], "~@") || str_eq(l[i-1], "'") || str_eq(l[i-1], "~") || str_eq(l[i-1], "`"))){
                current_list_pointer = cons(cons(parser_get_tag(l[i-1]),
                                                 cons(temp,
                                                      GLOBAL_NULL)),
                                            current_list_pointer);
                i--;
            }
            else{
                current_list_pointer = cons(temp, current_list_pointer);
            }
        }
    }

    // after parsing, free lexer
    Lexer_free(le);
    return current_list_pointer;
}

#if PARSER_DEBUG
void parser_debug (Object * p){
    printf("(");
    Object * v;
    while(p!=GLOBAL_NULL){
        v = car(p);
        if(v->type == PAIR || v == GLOBAL_NULL){
            parser_debug(v);
        }
        else{
            switch(v->type){
                case STRING:
                    printf("%s", v->data.String.v);
                    break;
                case INTEGER: 
                    printf("%ld", v->data.Integer.v);
                    break;
                case DOUBLE:
                    printf("%lf", v->data.Double.v);
                    break;
                case NULL_:
                    printf("()");
                    break;
                default:
                    break;
            }
        }
        printf(" ");
        p = cdr(p);
    }
    printf(")");
}
/*
int main(){
    char s[1000] = "a";
    Lexer * p;
    p = lexer((char*)s);
    Lexer_Debug(p);
    
    Object * o;
    o = parser(p);
    return 0;
}*/
#endif
#endif
