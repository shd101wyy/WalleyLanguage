/*
 walley language 0.3 by Yiyi Wang @ 2014
 Released under MIT Licenses
 lexer
 */
#include "builtin_procedures.h"
#ifndef LEXER_C
#define LEXER_C

// lexer struct
struct Lexer{
    char ** string_array;
    uint32_t array_length;
    uint32_t array_size;
};

// init lexer
Lexer * Lexer_init(){
    Lexer * l;
    l = (Lexer*)malloc(sizeof(Lexer));
    l->array_length = 0; // set length
    l->string_array = (char**)malloc(sizeof(char*)*1000);
    l->array_size = 1000;
    return l;
}

// push value to lexer
void Lexer_push(Lexer * l, char * value){
    if(l->array_length == l->array_size){
        // printf("realloc");
        l->array_size *= 2;
        l->string_array = realloc(l->string_array, sizeof(char*) * l->array_size);
    }
    char * s = malloc(sizeof(char)*((uint32_t)strlen(value) + 1));
    strcpy(s, value); // copy string
    l->string_array[l->array_length] = s; // push to lexer
    l->array_length += 1;
}

void Lexer_set(Lexer * l, uint32_t index, char * value){
    free(l->string_array[index]);
    l->string_array[index] = NULL;
    char * s = malloc(sizeof(char)*((uint32_t)strlen(value) + 1));
    strcpy(s, value); // copy string
    l->string_array[index] = s;
    return;
}

// free lexer
void Lexer_free(Lexer * l){
    uint64_t i;
    for(i = 0; i < l->array_length; i++){
        free(l->string_array[i]);
        l->string_array[i] = NULL;
    }
    free(l->string_array);
    l->string_array = NULL;
    free(l);
    l = NULL;
    return;
}

// string slice
char * string_slice(char * input_string, uint32_t start, uint32_t end){
    char *r = malloc(sizeof(char)*(end - start + 1));
    uint32_t i;
    for(i = start; i < end; i++){
        r[i-start] = input_string[i];
    }
    r[i-start] = 0;
    return r;
}
// string append
char * string_append(char *str1, char *str2){
    uint64_t length1 = strlen(str1);
    uint64_t length2 = strlen(str2);
    uint32_t i ;
    uint32_t j ;
    char *r = malloc(sizeof(char)*(length1 + length2 + 1));
    for (i = 0; i < length1; i++) {
        r[i] = str1[i];
    }
    for (j = 0; j < length2; j++) {
        r[i] = str2[j];
        i++;
    }
    r[i] = 0;
    return r;
}


//#define Lexer_set(l, index, value) ((l)->string_array[(index)] = (value))
#define Lexer_get(l, index) ((l)->string_array[(index)])
#define Lexer_length(l) ((l)->array_length)
#define true 1
#define false 0
/*
 Lexer
 which return an array of string
 */
Lexer* lexer(char * input_string){
    int32_t i = 0;
    int32_t j;
    uint32_t string_length = (uint32_t)strlen(input_string);
    // char * t;
    Lexer * output_list = Lexer_init();
    uint32_t start_index;
    uint32_t count;
    uint32_t a;
    uint32_t end;
    
    uint32_t paren_count = 0; // 确保 () match
    
    char * t;
    for(i = 0; i < string_length; i++){
        // printf("%d %s %c\n", i, input_string, input_string[i]);
        if(input_string[i] == '('){
            Lexer_push(output_list, "(");
            paren_count++;
        }
        else if (input_string[i] == ')'){
            Lexer_push(output_list, ")");
            paren_count--;
        }
        else if (input_string[i] == ' ' || input_string[i] == '\n' || input_string[i] == '\t' || input_string[i] == ','){
            continue;
        }
        else if (input_string[i] == '#' && (input_string[i + 1] == '[' || input_string[i + 1] == '(')){ // vector
            Lexer_push(output_list, "(");
            paren_count++;
            if (input_string[i + 1] == '(') {
                Lexer_push(output_list, "vector"); // 定长 vector, 长度不可变
            }
            else{
                Lexer_push(output_list, "vector!"); // 不定长 vector, 长度可变
            }
            i++;
        }
        else if (input_string[i] == '[' || input_string[i] == '{'){
            if(i!= 0 &&
               (input_string[i-1] != ' ' && input_string[i-1] != '\n'  && input_string[i-1] != '\t'
                && input_string[i-1] != '\'' && input_string[i-1]!='`' && input_string[i-1]!='~'
                && input_string[i - 1]!='(' && input_string[i - 1]!='{' && input_string[i - 1]!='[')){
                   if(Lexer_get(output_list, output_list->array_length - 1)[0]!=')'){ // +[  => ( +
                       Lexer_push(output_list, Lexer_get(output_list, (Lexer_length(output_list) - 1)));
                       Lexer_set(output_list, Lexer_length(output_list)-2, "(");
                       paren_count++;
                   }
                   else{ // ahead is )
                       count = 1;
                       start_index = 0;
                       for(j = output_list->array_length - 2; j >= 0; j--){
                           if(input_string[j] == ')'){
                               count++ ;
                           }
                           else if(input_string[j] == '('){
                               count--;
                               if(count == 0){
                                   start_index = j;
                                   break;
                               }
                           }
                       }
                       Lexer_push(output_list, Lexer_get(output_list, (Lexer_length(output_list) - 1))); // save last
                       for(j = output_list->array_length - 2; j != start_index; j--){
                           Lexer_set(output_list, j, Lexer_get(output_list, j - 1)); // move elements back
                       }
                       Lexer_set(output_list, j, "(");
                   }
               }
            else{
                if(input_string[i] == '[')
                    Lexer_push(output_list, "(");
                else{
                    Lexer_push(output_list, "(");
                    Lexer_push(output_list, "table");
                }
                paren_count++;
            }
        }
        else if (input_string[i] == ']' || input_string[i] == '}'){
            Lexer_push(output_list, ")");
            paren_count--;
        }
        else if (input_string[i] == '~' && input_string[i + 1] == '@'){
            Lexer_push(output_list, "~@");
            i++;
        }
        else if (input_string[i] == '\''){
            Lexer_push(output_list, "'");
        }
        else if (input_string[i] == '`'){
            Lexer_push(output_list, "`");
        }
        else if (input_string[i] == '~'){
            Lexer_push(output_list, "~");
        }
        else if (input_string[i] == ';') { // comment
            while (i != string_length) {
                if (input_string[i] == '\n') break;
                i++;
            }
        }
        else if (input_string[i] == '"') {
            a = i + 1;
            while (a != string_length) {
                if (input_string[a] == '\\') {
                    a = a + 2;
                    continue;
                }
                if (input_string[a] == '"') break;
                a++;
            }
            t = string_slice(input_string, i, a+1);
            Lexer_push(output_list, t);
            i = a;
            free(t);
        }
        else{
            end = i;
            while (1) {
                if (end == string_length || input_string[end] == ' ' || input_string[end] == '\n' || input_string[end] == '\t' || input_string[end] == ',' || input_string[end] == ')' || input_string[end] == '(' || input_string[end] == ']' || input_string[end] == '[' || input_string[end] == '{' || input_string[end] == '}' || input_string[end] == '\'' || input_string[end] == '`' || input_string[end] == '~' || input_string[end] == ';') break;
                end += 1;
            }
            t = string_slice(input_string, i, end);
            Lexer_push(output_list, t);
            i = end - 1;
            free(t);
        }
    }
    if (paren_count!=0) {
        printf("ERROR: parentheses () num doesn't match");
        Lexer_free(output_list);
        return NULL;
    }
    return output_list;
}




#if LEXER_DEBUG

// print all strings in lexer string array
void Lexer_Debug(Lexer * l){
    uint32_t i;
    for(i = 0; i < l->array_length; i++){
        printf("%s ", l->string_array[i]);
    }
    printf("\nFinish Lexer_Debug\n");
}
/*
 // test
 int main(){
 char test[1000] = "(def x 12) (def y 5)";
 Lexer_Debug(lexer((char*)test));
 return 0;
 }*/

#endif
#endif
