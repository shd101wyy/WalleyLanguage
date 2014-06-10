//
//  to_string.h
//  walley
//
//  Created by WangYiyi on 6/10/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_to_string_h
#define walley_to_string_h

#include "data_list.h"

#define TO_STRING_BUFFER_SIZE 4098
/*
 Data to string
 */
char * list_to_string(Data * l);
char * vector_to_string(Data * l);
char * table_to_string(Data * t);
char * clean_string(Data * s);

char * number_to_string(Data * x){
    char buffer[TO_STRING_BUFFER_SIZE];
    char * output = NULL;
    strcpy(buffer, "");
    
    switch (x->type) {
        case INTEGER:
            sprintf(buffer, "%lld", (long long int)x->data.Integer.v);
            break;
        case FLOAT:
            sprintf(buffer, "%lf", x->data.Float.v);
            break;
        //case RATIO:
        //    sprintf(buffer, "%lld/%lld", (long long int)x->data.Ratio.numer, (long long int)x->data.Ratio.denom);
        //    break;
        default:
            output = malloc(sizeof(char) * 3);
            strcpy(output, "()");
            return output;
    }
    output = malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(output, buffer);
    return output;
}

char * clean_string(Data * s){
    char buffer[TO_STRING_BUFFER_SIZE];
    strcpy(buffer, "");
    
    uint64_t i = 0;
    uint64_t length = s->actions[STRING_LENGTH_OFFSET]->data.Integer.v;
    char c;
    int32_t has_space = 0;
    char temp_buffer[1]; // single char buffer
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
    char * return_s = malloc(sizeof(char) * (strlen(buffer) + 1));
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


char * list_to_string(Data * l){
    char buffer[TO_STRING_BUFFER_SIZE]; // maximum 1024
    char address_buffer[12];
    strcpy(buffer, "(");
    
    Data * p = l;
    Data * v;
    char * s;
    /*
     这里可能得free .
     */
    while (p != GLOBAL_NULL) {
        if (p->type != LIST) {
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
            case INTEGER: case FLOAT: // case RATIO:
                s = number_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case STRING:
                s = clean_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case LIST:
                s = list_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case FN:
                strcat(buffer, "< user-defined-fn >");
                break;
            case BUILTIN_FN:
                strcat(buffer, "< builtin-fn >");
                break;
                /*
            case VECTOR:
                s = vector_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
            case TABLE:
                s = table_to_string(v);
                strcat(buffer, s);
                free(s);
                break;
                 */
            case OBJECT:
                strcat(buffer, "< ");
                strcat(buffer, Object_getSlot(v, STRING_type)->data.String.v); // get type
                strcat(buffer, " 0x");
                sprintf(address_buffer, "%x", (uint32_t)(v));
                strcat(buffer, address_buffer);
                strcat(buffer, " >");
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
    char * return_s = malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(return_s, buffer);
    return return_s;
}
/*
 * Object to string
 */
char * object_to_string(Data * o){
    char buffer[TO_STRING_BUFFER_SIZE]; // maximum 1024
    char address_buffer[12];
    
    Data ** msgs = o->msgs;
    Data ** actions = o->actions;
    uint16_t length = o->length;
    uint16_t i;
    
    // eg:  ===> Dog 0x43fea123
    strcpy(buffer, "===> ");
    strcat(buffer, Object_getSlot(o, STRING_type)->data.String.v);
    strcat(buffer, " 0x");
    sprintf(address_buffer, "%x", (uint32_t)(o));
    strcat(buffer, address_buffer);
    strcat(buffer, "\n");
    
    for (i = 0; i < length; i++) {
        strcat(buffer, "      ");
        strcat(buffer, msgs[i]->data.String.v);
        strcat(buffer, "\n");
        strcat(buffer, "             =");
        if (actions[i]->type == OBJECT) {
            strcat(buffer, "< ");
            strcat(buffer, Object_getSlot(actions[i], STRING_type)->data.String.v); // get type
            strcat(buffer, " 0x");
            sprintf(address_buffer, "%x", (uint32_t)(actions[i]));
            strcat(buffer, address_buffer);
            strcat(buffer, " >\n");
        }
        else{
            strcat(buffer, to_string(actions[i]));
            strcat(buffer, "\n");
        }
    }
    
    
    char * return_s = malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(return_s, buffer);
    return return_s;
}


char * to_string(Data * v){
    char buffer[TO_STRING_BUFFER_SIZE];
    strcpy(buffer, "");
    char * s;
    switch (v->type) {
        case NULL_:
            strcat(buffer, "()");
            break;
        case INTEGER: case FLOAT: // case RATIO:
            s = number_to_string(v);
            strcat(buffer, s);
            free(s);
            break;
        case STRING:
            strcat(buffer, v->data.String.v);
            break;
        case LIST:
            s = list_to_string(v);
            strcat(buffer, (s));
            free(s);
            break;
        case FN:
            strcat(buffer, "< user-defined-fn >");
            break;
        case BUILTIN_FN:
            strcat(buffer, "< builtin-fn >");
            break;
            /*
        case VECTOR:
            s = vector_to_string(v);
            strcat(buffer, (s));
            free(s);
            break;
        case TABLE:
            s = table_to_string(v);
            strcat(buffer, s);
            free(s);
            break;
             */
        case OBJECT:
            s = object_to_string(v);
            strcat(buffer, s);
            free(s);
            break;
        default:
            printf("ERROR: stdout");
            break;
    }
    char * return_s = malloc(sizeof(char) * (strlen(buffer) + 1));
    strcpy(return_s, buffer);
    return return_s;
}

#endif
