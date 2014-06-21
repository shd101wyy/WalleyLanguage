/*
 * 
 *  helper .c
 *  create new .h file which include the content of walley_core.wa
 *
 */

#include "vm.h"
#ifndef HELPER_H
#define HELPER_H 1

 void parser_debug_helper (FILE * file, Object * p){
    fprintf(file, "(");
    Object * v;
    uint64_t i;
    while(p!=GLOBAL_NULL){
        v = car(p);
        if(v->type == PAIR || v == GLOBAL_NULL){
            parser_debug_helper(file, v);
        }
        else{
            switch(v->type){
                case STRING:
                    for (i = 0; i < strlen(v->data.String.v); i++){
                        if (v->data.String.v[i] == '"'){
                            fputc('\\', file);
                        }
                        fputc(v->data.String.v[i], file);
                    }
                    break;
                case INTEGER: 
                    fprintf(file, "%lld", v->data.Integer.v);
                    break;
                case DOUBLE:
                    fprintf(file, "%lf", v->data.Double.v);
                    break;
                case NULL_:
                    fprintf(file, "()");
                    break;
                default:
                    break;
            }
        }
        fprintf(file, " ");
        p = cdr(p);
    }
    fprintf(file, ")");
}

 int main(int argc, char *argv[]){

        // init several constants
    GLOBAL_NULL = Object_initNull(); // init GLOBAL_NULL
    GLOBAL_NULL->use_count = 1;
    
    QUOTE_STRING = Object_initString("quote", 5); // 0
    QUOTE_STRING->use_count = 1;
    
    UNQUOTE_STRING = Object_initString("unquote", 7); // 1
    UNQUOTE_STRING->use_count = 1;
    
    UNQUOTE_SPLICE_STRING = Object_initString("unquote-splice", 14); // 2
    UNQUOTE_SPLICE_STRING->use_count = 1;
    
    QUASIQUOTE_STRING = Object_initString("quasiquote", 10); // 3
    QUASIQUOTE_STRING->use_count = 1;
    
    CONS_STRING = Object_initString("cons", 4); // 4
    CONS_STRING->use_count = 1;
    
    DEF_STRING = Object_initString("def", 3); // 5
    DEF_STRING->use_count = 1;
    
    SET_STRING = Object_initString("set!", 4); // 6
    SET_STRING->use_count = 1;
    
    LAMBDA_STRING = Object_initString("lambda", 6); // 7
    LAMBDA_STRING->use_count = 1;
    
    GLOBAL_TRUE = Object_initString("true", 4); // 8
    GLOBAL_TRUE->use_count = 1;
    
    INTEGER_STRING = Object_initString("integer", 7);// 9
    INTEGER_STRING->use_count = 1;
 
    FLOAT_STRING = Object_initString("float", 5); // 10
    FLOAT_STRING->use_count = 1;
    
    RATIO_STRING = Object_initString("ratio", 5); // 11
    RATIO_STRING->use_count = 1;
    
    STRING_STRING = Object_initString("string", 6); // 12
    STRING_STRING->use_count = 1;
    
    PAIR_STRING = Object_initString("pair", 4); // 13
    PAIR_STRING->use_count = 1;
    
    VECTOR_STRING = Object_initString("vector", 6); // 14
    VECTOR_STRING->use_count = 1;
    
    TABLE_STRING = Object_initString("table", 5); // 15
    TABLE_STRING->use_count = 1;
    
    CLONE_STRING = Object_initString("clone", 5); // 16
    CLONE_STRING->use_count = 1;
    
    STRING_proto = Object_initString("proto", 5); // 17
    STRING_proto->use_count = 1;
    
    STRING_type = Object_initString("type", 4); // 18
    STRING_type->use_count = 1;
    
    STRING_object = Object_initString("object", 6); // 19
    STRING_object->use_count = 1;



  /*
   *
   *   read walley_core.wa
   *
   */
   uint16_t i;
    // init sys_argv
   SYS_ARGV = Object_initVector(0, argc);
   SYS_ARGV->data.Vector.length = argc;
    SYS_ARGV->use_count = 1; // in use
    for(i = 0; i < argc; i++){
        vector_Set(SYS_ARGV, i, Object_initString(argv[i], strlen(argv[i])));
    }

    // ################# HELPER ################################
    FILE * file = fopen("walley_core.wa", "r");
    if(!file){
        printf("ERROR: Failed to include walley_core.wa file\n");
        return 0;
    }
    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);
    
    char * content = calloc(size + 1, 1);
    fread(content, 1, size, file);
    
    fclose(file);
    
    Lexer * p;
    Object * o;
    char * temp;
    //printf("content %s\n", content);
    //printf("lexer:\n");
    p = lexer(content);
    //printf("parser:\n");
    o = parser(p);
    FILE * write_file = fopen("walley_core.h", "w");
    fprintf(write_file, "// helper file, load walley_wore.wa\n");
    fprintf(write_file, "char WALLEY_CORE_CONTENT[] = \"");
    while(o!=GLOBAL_NULL){
        parser_debug_helper(write_file, car(o)); // write to file
        o = cdr(o);
    }
    fprintf(write_file, "\";");
    /*
     printf("get o %s\n",to_string(car(o)));
     
     }
     printf("Helper .h");
     */
     return 0;
    // ######################################################

 }

#endif