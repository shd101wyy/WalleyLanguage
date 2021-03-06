//
//  to_string.h
//  walley
//
//  Created by WangYiyi on 4/30/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_to_string_h
#define walley_to_string_h
#include "ratio.h"

#define TO_STRING_BUFFER_SIZE 4098
/*
    Object to string
*/
char * list_to_string(Object * l);
char * vector_to_string(Object * l);
char * table_to_string(Object * t);
char * clean_string(Object * s);
char * to_string(Object * v);
char * number_to_string(Object * x){
    char buffer[TO_STRING_BUFFER_SIZE];
    strcpy(buffer, "");

    switch (x->type) {
        case INTEGER:
            sprintf(buffer, "%lld", (long long int)x->data.Integer.v);
            break;
        case DOUBLE_:
            sprintf(buffer, "%lf", x->data.Double.v);
            break;
        case RATIO:
            if (x->data.Ratio.denom < 0) { // fix (/ -14 18) => 7/-9 bug
                sprintf(buffer, "-%lld/%lld", (long long int)x->data.Ratio.numer, -(long long int)x->data.Ratio.denom);

            }
            else{
                sprintf(buffer, "%lld/%lld", (long long int)x->data.Ratio.numer, (long long int)x->data.Ratio.denom);
            }
            break;
        default:
            return "()";
    }
    char * output = (char*)malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(output, buffer);
    return output;
}
char * clean_string(Object * s){
    char buffer[TO_STRING_BUFFER_SIZE] = "";

    uint64_t i = 0;
    uint64_t length = s->data.String.length;
    char c;
    int32_t has_space = 0;
    char temp_buffer[2]; // single char buffer
    temp_buffer[1] = '\0';
    for (i = 0; i < length; i++) {
        c = s->data.String.v[i];
        if (c == ' ' || c == '\n' || c == '\t' || c == '\a') {
            has_space = 1; break;
        }
    }
    if (has_space) {
        strcat(buffer, "\"");
        for (i = 0; i < length; i++) {
            c = s->data.String.v[i];
            switch (c) {
                case '\n':
                    strcat(buffer, "\\n");
                    break;
                case '\t':
                    strcat(buffer, "\\t");
                    break;
                    //case '\\':
                case '\a':
                    strcat(buffer, "\\a");
                    break;
                default:
                    temp_buffer[0] = c;
                    strcat(buffer, temp_buffer);
                    break;
            }
        }
        //strcat(buffer, "#str{");
        //strcat(buffer, s->data.String.v);
        //strcat(buffer, "}");
        strcat(buffer, "\"");
    }
    else{
        strcat(buffer, s->data.String.v);
    }
    char * return_s = (char*)malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(return_s, buffer);
    return return_s;
}
char * list_to_string(Object * l){
    char buffer[TO_STRING_BUFFER_SIZE]; // maximum 1024
    strcpy(buffer, "(");
    uint16_t a;
    Object * p = l;
    Object * v;
    char * s;
	uint16_t end_;
    /*
        这里可能得free .
     */
    while (p != GLOBAL_NULL) {
        if (p->type != PAIR) {
            v = p;
            strcat(buffer, ". ");
            p = cons(p, GLOBAL_NULL);
        }
        else
            v = car(p);
        switch (v->type) {
            case NULL_:
                strcat(buffer, "()");
                break;
            case INTEGER: case DOUBLE_: case RATIO:
                s = number_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case STRING:
                s = clean_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case PAIR:
                s = list_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case USER_DEFINED_LAMBDA:
                strcat(buffer, "#<user-defined-lambda (");
                end_ = (v->data.User_Defined_Lambda.variadic_place >= 0 ? (v->data.User_Defined_Lambda.param_num - 1) : v->data.User_Defined_Lambda.param_num);
                for (a = 0; a < end_; a++) {
                    strcat(buffer, "_");
                    if (a != end_ - 1) {
                        strcat(buffer, " ");
                    }
                }
                if (v->data.User_Defined_Lambda.variadic_place >= 0) {
                    strcat(buffer, " . _");
                }
                strcat(buffer, ")>");
                break;           
			case BUILTIN_LAMBDA:
                strcat(buffer, "#<builtin-lambda>");
                break;
            case VECTOR:
                s = vector_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case TABLE: case OBJECT:
                s = table_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case CONTINUATION:
                strcat(buffer, "#<continuation>");
                break;
            default:
                printf("ERROR: stdout");
                break;
        }
        p = cdr(p);
        if (p!=GLOBAL_NULL) {
            strcat(buffer, " ");
        }
    }
    strcat(buffer, ")");
    char * return_s = (char*)malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(return_s, buffer);
    return return_s;
}
char * vector_to_string(Object * l){
    char buffer[TO_STRING_BUFFER_SIZE]; // maximum 1024
    strcpy(buffer, "");

    Object * v;
    /*
     这里可能得free .
     */
    uint64_t length = l->data.Vector.length;
    uint64_t i;
    uint16_t a;
    char * s;
	uint16_t end_;
    if (l->data.Vector.resizable) {
        strcat(buffer, "[");
    }
    else strcat(buffer, "#(");
    
    for (i = 0; i < length; i++) {
        v = l->data.Vector.v[i];
        switch (v->type) {
            case NULL_:
                strcat(buffer, "()");
                break;
            case INTEGER: case DOUBLE_: case RATIO:
                s = number_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case STRING:
                s = clean_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case PAIR:
                s = list_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case USER_DEFINED_LAMBDA:
                strcat(buffer, "#<user-defined-lambda (");
                end_ = (v->data.User_Defined_Lambda.variadic_place >= 0 ? (v->data.User_Defined_Lambda.param_num - 1) : v->data.User_Defined_Lambda.param_num);
                for (a = 0; a < end_; a++) {
                    strcat(buffer, "_");
                    if (a != end_ - 1) {
                        strcat(buffer, " ");
                    }
                }
                if (v->data.User_Defined_Lambda.variadic_place >= 0) {
                    strcat(buffer, " . _");
                }
                strcat(buffer, ")>");
                break;
            case BUILTIN_LAMBDA:
                strcat(buffer, "#<builtin-lambda>");
                break;
            case VECTOR:
                s = vector_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case TABLE: case OBJECT:
                s = table_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case CONTINUATION:
                strcat(buffer, "#<continuation>");
                break;
            default:
                printf("ERROR: stdout");
                break;
        }
        if (i!=length - 1) {
            strcat(buffer, " ");
        }

    }
    if (l->data.Vector.resizable) {
        strcat(buffer, "]");
    }
    else{
        strcat(buffer, ")");
    }
    char * return_s = (char*)malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(return_s, buffer);
    return return_s;
}

char * table_to_string(Object * l){
    char buffer[TO_STRING_BUFFER_SIZE]; // maximum 1024
    strcpy(buffer, "");
    
    Object * v;
    char * s;
    Object * keys = table_getKeys(l); // it is pair
    strcat(buffer, "{");
    uint16_t a;
	uint16_t end_;
    while (keys!=GLOBAL_NULL) {
        v = Table_getval(l, car(keys));
        if(car(keys)->type == STRING)
            strcat(buffer, ":");
        strcat(buffer, to_string(car(keys)));
        strcat(buffer, " ");
        switch (v->type) {
            case NULL_:
                strcat(buffer, "()");
                break;
            case INTEGER: case DOUBLE_: case RATIO:
                s = number_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case STRING:
                s = clean_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case PAIR:
                s = list_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case USER_DEFINED_LAMBDA:
                strcat(buffer, "#<user-defined-lambda (");
                end_ = (v->data.User_Defined_Lambda.variadic_place >= 0 ? (v->data.User_Defined_Lambda.param_num - 1) : v->data.User_Defined_Lambda.param_num);
                for (a = 0; a < end_; a++) {
                    strcat(buffer, "_");
                    if (a != end_ - 1) {
                        strcat(buffer, " ");
                    }
                }
                if (v->data.User_Defined_Lambda.variadic_place >= 0) {
                    strcat(buffer, " . _");
                }
                strcat(buffer, ")>");
                break;
            case BUILTIN_LAMBDA:
                strcat(buffer, "#<builtin-lambda>");
                break;
            case VECTOR:
                s = vector_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case TABLE: case OBJECT:
                s = table_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case CONTINUATION:
                strcat(buffer, "#<continuation>");
                break;
            default:
                printf("ERROR: stdout");
                break;
        }

        keys = cdr(keys);
        if (keys!=GLOBAL_NULL) {
            strcat(buffer, ", ");
        }
    }
    strcat(buffer, "}");
    char * return_s = (char*)malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(return_s, buffer);
    return return_s;
}

char * to_string(Object * v){
    char buffer[TO_STRING_BUFFER_SIZE];
    strcpy(buffer, "");
    char * s;
    uint16_t a;
	uint16_t end_;
    switch (v->type) {
        case NULL_:
            strcat(buffer, "()");
            break;
        case INTEGER: case DOUBLE_: case RATIO:
            s = number_to_string(v);
            strcat(buffer, s);
            free(s);
            break;
        case STRING:
            strcat(buffer, v->data.String.v);
            break;
        case PAIR:
            s = list_to_string(v);
            strcat(buffer, (s));
            free(s);
            break;
        case USER_DEFINED_LAMBDA:
            strcat(buffer, "#<user-defined-lambda (");
            end_ = (v->data.User_Defined_Lambda.variadic_place >= 0 ? (v->data.User_Defined_Lambda.param_num - 1) : v->data.User_Defined_Lambda.param_num);
            for (a = 0; a < end_; a++) {
                strcat(buffer, "_");
                if (a != end_ - 1) {
                    strcat(buffer, " ");
                }
            }
            if (v->data.User_Defined_Lambda.variadic_place >= 0) {
                strcat(buffer, " . _");
            }
            strcat(buffer, ")>");
            break;
        case BUILTIN_LAMBDA:
            strcat(buffer, "#<builtin-lambda>");
            break;
        case VECTOR:
            s = vector_to_string(v);
            strcat(buffer, (s));
            free(s);
            break;
        case TABLE: case OBJECT:
            s = table_to_string(v);
            strcat(buffer, s);
            free(s);
            break;
        case FILE_:
            strcat(buffer, "<File ");
            // strcat(buffer,);
            strcat(buffer, ">");
            break;
        case CONTINUATION:
            strcat(buffer, "#<continuation>");
            break;
        default:
            printf("ERROR: stdout");
            break;
    }
    char * return_s = (char*)malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(return_s, buffer);
    return return_s;
}

/*
 * format string
 * eg ""Hello World"" -> Hello World
 * don't forget to free the created string
 */
char * format_string(char * input_str){
    char * s = (char*)malloc(sizeof(char)*(strlen(input_str)+1));
    int32_t j = 0;
    int32_t i;
    // format string
    for (i = 1; i < strlen(input_str)-1; i++) {
        if(input_str[i] == '\\'){
            switch (input_str[i+1]) {
                case 'a':
                    s[j] = '\a';
                    break;
                case 't':
                    s[j] = '\t';
                    break;
                case 'n':
                    s[j] = '\n';
                    break;
                case '\\':
                    s[j] = '\\';
                    break;
                case '"':
                    s[j] = '"';
                    break;
                default:
                    printf("ERROR: Invalid String Slash\n");
                    break;
            }
            i++;
        }
        else
            s[j] = input_str[i];
        j++;
    }
    s[j] = 0; // end of string
    return s;
}

#endif
