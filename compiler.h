//
//  compiler.h
//  walley
//
//  Created by WangYiyi on 4/27/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_compiler_h
#define walley_compiler_h
#include "env_data_type.h"

// static int32_t CONSTANT_TABLE_LENGTH = 0;
int16_t compiler(Instructions * insts,
              Object * l,
              Variable_Table * vt,
              int32_t tail_call_flag,
              char * parent_func_name,
              Lambda_for_Compilation * function_for_compilation,
              Environment * env,
              MacroTable * mt,
              Module * module);

Object * compiler_begin(Instructions * insts,
                        Object * l,
                        Variable_Table * vt,
                        char * parent_func_name,
                        Lambda_for_Compilation * function_for_compilation,
                        int32_t eval_flag,
                        Environment * env,
                        MacroTable * mt,
                        Module * module);

Object *VM(/*uint16_t * instructions,*/
           Instructions * instructions_,
           uint64_t start_pc,
           uint64_t end_pc,
           Environment * env,
           Variable_Table * vt,
           MacroTable * mt,
           Module * module,
           Object * default_acc,
           Object * continuation_state);

Object * quote_list(Object * l){
    if(l->type == NULL_) return GLOBAL_NULL;
    Object * v = car(l);
    if(v->type == PAIR) return cons(CONS_STRING,
                                    cons(quote_list(v),
                                         cons(quote_list(cdr(l)),
                                              GLOBAL_NULL)));
    else if (v->type == STRING && str_eq(v->data.String.v, "."))
        return cons(QUOTE_STRING, cons(cadr(l), GLOBAL_NULL));
    return cons(CONS_STRING,
                cons(cons(QUOTE_STRING, cons(v, GLOBAL_NULL)),
                     cons(quote_list(cdr(l)), GLOBAL_NULL)));
}

Object * quasiquote_list(Object * l){
    if (l->type == NULL_) {
        return GLOBAL_NULL;
    }
    Object * v = car(l);
    if(v->type == PAIR){
        if (car(v)->type == STRING
            && str_eq(car(v)->data.String.v, "unquote")) {
            return cons(CONS_STRING,
                        cons(cadr(v),
                             cons(quasiquote_list(cdr(l)),
                                  GLOBAL_NULL)));
        }
        else if (car(v)->type == STRING
                 && str_eq(car(v)->data.String.v, "unquote-splice")){
            return cons(Object_initString("append", 6),
                        cons(cadr(v),
                             cons(quasiquote_list(cdr(l)),
                                  GLOBAL_NULL)));
        }
        return cons(CONS_STRING,
                    cons(quasiquote_list(v),
                         cons(quasiquote_list(cdr(l)), GLOBAL_NULL)));
    }
    else if (v->type == STRING && str_eq(v->data.String.v, "."))
        return cons(QUOTE_STRING, cons(cadr(l), GLOBAL_NULL));
    else
        return cons(CONS_STRING,
                    cons(cons(QUOTE_STRING,
                              cons(v, GLOBAL_NULL)),
                         cons(quasiquote_list(cdr(l)),
                              GLOBAL_NULL)));
}

/*
* This function si used in macro_match function
* when macro doesn't match, free stored varnames. 
*/
void free_varnames(char **var_names, int32_t count){
    int16_t i;
    for(i = 0; i < count; i++){
        free(var_names[i]);
    }
}
/*
 macro_match
 return length of var_names
 (defmacro test 
     (x) x 
     (#hi x) y
     (x y) x)
 
 */
int32_t macro_match(Object * a, Object * b, char **var_names, Object **var_values, int32_t count){
#if MACRO_DEBUG
    printf("\nMacro Match\n");
    printf("a:  \n");
    parser_debug(a);
    printf("\nb:  \n");
    parser_debug(b);
#endif
    if (a->type == NULL_ && b->type == NULL_) {
        return count; // match
    }
    else if ((a == GLOBAL_NULL && b != GLOBAL_NULL)
             || (a != GLOBAL_NULL && b == GLOBAL_NULL && (!str_eq(car(a)->data.String.v, ".")))){ // here is to fix a => (. args) b => (), so "args" should match ()
        free_varnames(var_names, count);
        return 0; // doesn't match
    }
    else if(car(a)->type == PAIR && car(b)->type == PAIR){
        int32_t match = macro_match(car(a), car(b), var_names, var_values, count);
        if (!match){
            free_varnames(var_names, count);
            return 0; // doesn't match
        } 
        return macro_match(cdr(a),
                           cdr(b),
                           var_names,
                           var_values,
                           match);
    }
    else if(car(a)->type == PAIR && car(b)->type != PAIR){
        free_varnames(var_names, count);
        return 0;
    }
    else{
        if (car(a)->type == STRING && car(a)->data.String.v[0] == '#') {
            // constant
            if(car(b)->type != STRING){
                free_varnames(var_names, count);
                return 0; // doesn't match
            } 
            if (strcmp( &(*((char*)car(a)->data.String.v + 1)), car(b)->data.String.v) == 0) {
                return macro_match(cdr(a),
                                   cdr(b),
                                   var_names,
                                   var_values,
                                   count);
            }
            else{
                free_varnames(var_names, count);
                return 0;
            }
        }
        if (car(a)->type == STRING &&
            str_eq(car(a)->data.String.v, ".")) {
            // 这里 var_name 前加 空格
            char * temp = (char*)malloc(sizeof(char) * (strlen(cadr(a)->data.String.v) + 2));
            strcpy(temp, " ");
            strcat(temp, cadr(a)->data.String.v);
            var_names[count] = temp;// cadr(a)->data.String.v; // save var name
            var_values[count] = b; // save var value
            count++;
            return count;
        }
        else{
            char * temp = (char*)malloc(sizeof(char) * (strlen(car(a)->data.String.v) + 1));
            strcpy(temp, car(a)->data.String.v); 
            var_names[count] = temp; // save var_name
            var_values[count] = car(b); // save var_value
            count++;
            return macro_match(cdr(a),
                               cdr(b),
                               var_names,
                               var_values,
                               count);
        }
    }
}

Object * macro_expansion_replacement(Object * expanded_value,
                                     Variable_Table * vt,
                                     int32_t is_head,
                                     Module * module){
    if (expanded_value == GLOBAL_NULL) {
        return GLOBAL_NULL;
    }
    if (expanded_value->type != PAIR) {
        return expanded_value;
    }
    Object * v = car(expanded_value);
    if (v->type == PAIR) {
        return cons(macro_expansion_replacement(v, vt, true, module),
                    macro_expansion_replacement(cdr(expanded_value),
                                                vt, false, module));
    }
    else{
        if (v->type == STRING && is_head) { // check in vt
            if (str_eq(v->data.String.v, "quote")
                ||str_eq(v->data.String.v, "quasiquote")) {// is quote function, so no replacement
                return expanded_value;
            }
            if (v->data.String.v[0] == '!') { // means not replace
                return cons(Object_initString(&(v->data.String.v[1]), strlen(v->data.String.v) - 1),
                            macro_expansion_replacement(cdr(expanded_value), vt, false, module));
            }
            int32_t find[2];
            VT_find(vt, v->data.String.v, find, module);
            // here is to solve (fn (args) ...). here args shouldn't be replaced.
            if ( (str_eq(v->data.String.v, "fn") || str_eq(v->data.String.v, "lambda")) && cadr(expanded_value)->type == PAIR ){
                return cons(v, cons(cadr(expanded_value),
                                    macro_expansion_replacement(cddr(expanded_value), vt, false, module)));
            }
            /*
             todo: change this VT_find later
             */
            else if (find[0] != -1) { // find
                return cons(cons(Object_initInteger(0),
                                 cons(Object_initInteger(find[0]),
                                      cons(Object_initInteger(find[1]),
                                           GLOBAL_NULL))),
                            macro_expansion_replacement(cdr(expanded_value),
                                                        vt, false, module));
            }
            else{
                return cons(v, macro_expansion_replacement(cdr(expanded_value), vt, false, module));
            }
        }
        else if(v->type == STRING ||
                v->type == INTEGER ||
                v->type == DOUBLE_ ||
                v->type == NULL_)
            return cons(v, macro_expansion_replacement(cdr(expanded_value), vt, false, module));
        else{
            printf("ERROR: Macro expansion failed. Invalid Data Type\n");
            printf("     :%s\n", to_string(v));
            return GLOBAL_NULL;
        }
    }
}

/*
 I need to think about it
 
 (defn test []
 (defn hi [] (display "Hi"))
 (defmacro t [] '(hi))
 (t)
 )
 (t) => cause error because hi is defined inside test
 */
/*
 展开 macro
 (defmacro square (x) `(* ~x ~x))
 (square 12) => (* 12 12)
 */
Object * macro_expand_for_compilation(Macro * macro, Object * exps, MacroTable * mt, Environment * global_env, Instructions * insts, Module * module){
    Object * clauses = macro->clauses;
    Object * expanded_value_after_replacement;
    Object * temp;
    // macro 最多有 64 个 parameters
    char * var_names[64];
    Object * var_values[64];
    int32_t match;
    int32_t i;
    uint64_t start_pc, insts_length;
    while (clauses != GLOBAL_NULL) {
        match = macro_match(car(clauses),
                            exps,
                            var_names,
                            var_values,
                            0);
        if (match || (exps == GLOBAL_NULL && car(clauses) == GLOBAL_NULL)) {
#if MACRO_DEBUG
            printf("Macro Match\n");
            printf("Match length %d\n", match);
#endif
            
            // return value
            Object * expanded_value;
            
            // 只用 env 和 vt 的第一个frame
            // 这里我手动写吧。。
            Variable_Table * new_vt;
            Environment * new_env;
            Variable_Table_Frame * new_vt_top_frame;
            Environment_Frame * new_env_top_frame;
            
            // init vt frame
			new_vt_top_frame = (Variable_Table_Frame*)malloc(sizeof(Variable_Table_Frame));
            // new_vt_top_frame->use_count = 1; // 后面要被使用
            new_vt_top_frame->var_names = (char**)malloc(sizeof(char*)*match);
            new_vt_top_frame->length = match; // set length directly
            new_vt_top_frame->use_count = 1; 
            
            // init vt
			new_vt = (Variable_Table*)malloc(sizeof(Variable_Table));
            new_vt->frames[0] = macro->vt->frames[0];
            new_vt->frames[1] = new_vt_top_frame;
            new_vt->length = 2; // set length
            
            // init env frame
            new_env_top_frame = EF_init_with_size(match+1);
            
            // init env
            new_env = Env_init_with_size(64);
            new_env->frames[0] = global_env->frames[0]; // 指向第一个frame
            new_env->frames[1] = new_env_top_frame;
            new_env->length = 2; // set length
            new_env_top_frame->use_count = 1 ;
            
            // add var name to top frame of new_vt;
            for (i = 0; i < match; i++) {
                if (var_names[i][0] == ' ') {
                    char * a = (char*)malloc(sizeof(char) * (strlen(var_names[i]) + 1));
                    strcpy(a, &var_names[i][1]);
                    free(var_names[i]);
                    var_names[i] = a;
                }
                else{
                    //
                    //    这里是解决以下的bug
                    //    (def x 12)
                    //    (defn t [x] x)
                    //    这里替换 [x] 的时候， 因为亦钱已经定义了x，且[x]的x在head， 所以会被替换。 其实这里不应该被替换
                    //
                    // no replacement
                    if (var_values[i]->type == PAIR && car(var_values[i])->type == STRING) {
                        temp = car(var_values[i]);
                        char * t = temp->data.String.v;
                        
                        // 不改变以下的这几个
                        if (str_eq(t, "quasiquote") || str_eq(t, "quote")
                            || str_eq(t, "unquote-splice")) {
                        }
                        else{
                            char buffer[256];
                            strcpy(buffer, "!");
                            strcat(buffer, car(var_values[i])->data.String.v);
                            car(var_values[i]) = Object_initString(buffer, strlen(buffer));
                            // free 掉原来的
                            free(temp->data.String.v);
                            free(temp);
                        }
                    }
                }
                
                // 这里不用 VT_push了因为不用copy string了
                new_vt_top_frame->var_names[i] = var_names[i];
                
                new_env_top_frame->array[i] = var_values[i];
                var_values[i]->use_count++; // in use
                
#if MACRO_DEBUG
                printf("\nVar-Name  :%s\n", var_names[i]);
                printf("Var-Value :\n");
                printf("%s\n", to_string(var_values[i]));
#endif
                
            }
            
            // save start-pc and length
            start_pc = insts->start_pc;
            insts_length = insts->length;
            
            
            // compile and run
            compiler_begin(insts,
                           cons(car(cdr(clauses)), GLOBAL_NULL)
                           ,
                           new_vt,
                           NULL,
                           NULL,
                           false,
                           new_env,
                           mt,
                           module);
            
            // cannot run in compiler_begin,
            // because the default insts->start_pc is wrong
            // should use insts_length as start_pc;
            expanded_value = VM(insts, insts_length, insts->length, new_env, NULL, NULL, module, GLOBAL_NULL, NULL);
            
#if MACRO_DEBUG
            printf("\n");
            printInstructions(insts);
            printf("\nFinish Expansion\n");
            // 这里结束可以用于 (macroexpand)
            printf("%s\n", to_string(expanded_value));
            //exit(0);
#endif
            // printf("%s\n", to_string(expanded_value));
            // restore start-pc and length
            insts->start_pc = start_pc;
            insts->length = insts_length;
            
            // 一改还得free其他的, var_values不用free
            // 因为 
            //     var_values 会随着 parser 而free掉
            // free new_vt
            // printf("\n match %d\n", match);
            for (i = 0; i < new_vt_top_frame->length; i++) {
             free(new_vt_top_frame->var_names[i]);
            }
            free(new_vt_top_frame->var_names);
            free(new_vt_top_frame);
            free(new_vt);


            // free new_env
            new_env_top_frame->use_count--;
            EF_free(new_env_top_frame);
            free(new_env->frames);
            free(new_env);
            
            // printf("expanded_value %s\n", to_string(expanded_value));
            // 假设运行完了得到了 expanded_value
            // 根据 macro->vt 替换首项
            expanded_value_after_replacement = macro_expansion_replacement(expanded_value, macro->vt, true, module);
            Object_free(expanded_value);
            
            // printf("after expand: %s %d\n", to_string(expanded_value_after_replacement), expanded_value_after_replacement->use_count);
            return expanded_value_after_replacement;
        }
        // free var_names; 
        
        clauses = cddr(clauses);
        continue;
    }
    printf("ERROR: Macro: %s expansion failed\n", macro->macro_name);
    return GLOBAL_NULL;
}


Object * list_append(Object * a, Object * b){
    if(a == GLOBAL_NULL) return b;
    return cons(car(a), list_append(cdr(a), b));
}

/*
 Compiler 暂不支持 Macro
 */
int16_t compiler(Instructions * insts,
              Object * l,
              Variable_Table * vt,
              int32_t tail_call_flag,
              char * parent_func_name,
              Lambda_for_Compilation * function_for_compilation,
              Environment * env,
              MacroTable * mt,
              Module * module){
    int32_t i, j;
    uint64_t index1, index2, index3, jump_steps, start_pc;
    char * string;
    uint64_t length;
    int32_t find_end;
    int32_t vt_find[2];
    int64_t int_;
    double double_;
    char * tag;
    Object * v;
    Object * var_name, * var_value;
    Object * test, * conseq, * alter;
    Object * temp;
    int32_t var_existed/*, var_index*/;
    Variable_Table_Frame * frame;
	uint64_t * unsigned_int_;
    switch (l->type) {
        case NULL_:
            Insts_push(insts, CONST_NULL); // push null;
            return 0;
        case INTEGER: // integer
            int_ = l->data.Integer.v;
            
            // if int_ is within [0 250), then load from constant pool
            if (int_ >= 0 && int_ < 250) {
                // load from constant pool
                Insts_push(insts, CONST_LOAD);
                Insts_push(insts, int_);
                return 0;
            }
            Insts_push(insts, CONST_INTEGER);
            Insts_push(insts, (0xFFFF000000000000ULL & (uint64_t)int_) >> 48);
            Insts_push(insts, (0x0000FFFF00000000ULL & (uint64_t)int_) >> 32);
            Insts_push(insts, (0x00000000FFFF0000 & int_) >> 16);
            Insts_push(insts, 0xFFFF & int_);
            return 0;
        case DOUBLE_:
            double_ = l->data.Double.v;
            
			unsigned_int_ = (uint64_t *)&double_; // get hex
            Insts_push(insts, CONST_FLOAT);
            Insts_push(insts, (0xFFFF000000000000ULL & (uint64_t)(*unsigned_int_)) >> 48);
            Insts_push(insts, (0x0000FFFF00000000ULL & (uint64_t)(*unsigned_int_)) >> 32);
            Insts_push(insts, (0x00000000FFFF0000 & (*unsigned_int_)) >> 16);
            Insts_push(insts, 0xFFFF & (*unsigned_int_));
            return 0;
            /*
             关于 string, 存在于 CONSTANT_TABLE_INSTRUCTIONS 而不是 insts
             */
        case STRING:
            if (l->data.String.v[0] == '"') { // string
                char * s = format_string(l->data.String.v);
                // init key save to 'v'
                length = strlen(s);
                v = Object_initString(s, length);
                var_value = Table_getval(CONSTANT_TABLE_FOR_COMPILATION,
                                         v);
                // check s in CONSTANT_TABLE_FOR_COMPILATION
                if(var_value!= GLOBAL_NULL){ // already exist
                    Insts_push(insts, CONST_LOAD); // load from table
                    Insts_push(insts, var_value->data.Integer.v);
                    // free 'v'
                    free(v->data.String.v);
                    free(v);
                    free(s);
                    return 0;
                }
                else{ // doesn't exist, save to table
                    Insts_push(insts, CONST_LOAD); // load from table
                    Insts_push(insts, CONSTANT_TABLE_FOR_COMPILATION_LENGTH);
                    
                    Table_setval(CONSTANT_TABLE_FOR_COMPILATION,
                                 v,
                                 Object_initInteger(CONSTANT_TABLE_FOR_COMPILATION_LENGTH));
                    // printf("%s doesn't exist %ld\n", s, CONSTANT_TABLE_FOR_COMPILATION_LENGTH);
                    CONSTANT_TABLE_FOR_COMPILATION_LENGTH++;
                }
                /*
                if(strcmp("你", s) == 0){
                    printf("找到了你\n");
                }*/
                
                Insts_push(CONSTANT_TABLE_INSTRUCTIONS, CONST_STRING);
                Insts_push(CONSTANT_TABLE_INSTRUCTIONS, length);
                find_end = false;
                for (i = 0; i < length; i = i + 2) {
                    if(i + 1 == length){
                        Insts_push(CONSTANT_TABLE_INSTRUCTIONS, ((int32_t)s[i] << 8) & 0xFF00);
                        find_end = true;
                        break;
                    }
                    else {
                        /*
                        if(strcmp("你", s) == 0){
                            printf("找到了你\n");
                            printf("%d\n", s[i]);
                            printf("%d\n", s[i + 1]);
                            printf("%x\n", (int16_t)s[i] << 8);
                            printf("%x\n", (uint32_t)((int32_t)s[i] << 8) | ((int32_t)s[i+1] & 0xFF));
                            printf("%d\n", (int8_t)(((((int16_t)s[i] << 8) | s[i + 1]) & 0xFF00) >> 8)  );
                        }*/
                        // 这里原来出错了, 现在支持负数
                        Insts_push(CONSTANT_TABLE_INSTRUCTIONS, (uint32_t)((int32_t)s[i] << 8) | ((int32_t)s[i+1] & 0xFF));
                    }
                }
                if(find_end == false){
                    Insts_push(CONSTANT_TABLE_INSTRUCTIONS, 0x0000); // add end
                }
                free(s);
                return 0;
            }
            else{
                VT_find(vt, l->data.String.v, vt_find, module);
                if(vt_find[0] == -1){
                    // variable doesn't exist
                    printf("ERROR: undefined variable %s\n", l->data.String.v);
                    if (str_eq(l->data.String.v, "recur")) {
                        printf("ERROR: invalid tail call: recur\n");
                    }
                    return 0;
                }
                Insts_push(insts, GET<<12 | vt_find[0]); // frame index
                Insts_push(insts, vt_find[1]); // value index
                
                // check continue and break
                if (str_eq(l->data.String.v, "break") || str_eq(l->data.String.v, "continue")) {
                    Insts_push(insts, RETURN << 12);
                }
                
                return 0;
            }
        case PAIR:
            if(car(l)->type == INTEGER && car(l)->data.Integer.v == 0){
                // set
                Insts_push(insts, (GET << 12) | (0x0FFF & cadr(l)->data.Integer.v));
                Insts_push(insts, caddr(l)->data.Integer.v);
                return 0;
            }
            tag = car(l)->data.String.v;
            if(str_eq(tag, "quote")){
                v = cadr(l);
                // check integer float null
                if(v->type == NULL_ || v->type == INTEGER || v->type == DOUBLE_){
                    return compiler(insts,
                                    v,
                                    vt,
                                    tail_call_flag,
                                    parent_func_name,
                                    function_for_compilation,
                                    env,
                                    mt,
                                    module);
                }
                else if (v->type == PAIR){ // pair
                    temp = quote_list(v);
                    compiler(insts,
                             temp,
                             vt,
                             tail_call_flag,
                             parent_func_name,
                             function_for_compilation,
                             env,
                             mt,
                             module);
                    Object_free(temp);
                    return 0;
                }
                else if(v->data.String.v[0] != '\''){
                    string = (char*)malloc(sizeof(char) * (2 + v->data.String.length + 1));
                    strcpy(string, "\"");
                    strcat(string, v->data.String.v);
                    strcat(string, "\"");
                    v = Object_initString(string,
                                          strlen(string));
                    compiler(insts,
                             v,
                             vt,
                             tail_call_flag,
                             parent_func_name,
                             function_for_compilation,
                             env,
                             mt,
                             module);
                    Object_free(v);
                    free(string);
                    return 0;
                }
                return compiler(insts, v, vt, tail_call_flag, parent_func_name, function_for_compilation,env,mt,module);
            }
            else if(str_eq(tag, "quasiquote")){
                v = cadr(l);
                // check integer float null
                if(v->type == NULL_ || v->type == INTEGER || v->type == DOUBLE_){
                    return compiler(insts,
                                    v,
                                    vt,
                                    tail_call_flag,
                                    parent_func_name,
                                    function_for_compilation,
                                    env,
                                    mt,
                                    module);
                }
                else if (v->type == PAIR){ // pair
                    temp = quasiquote_list(v);
                    compiler(insts,
                             temp,
                             vt,
                             tail_call_flag,
                             parent_func_name,
                             function_for_compilation,
                             env,
                             mt,
                             module);
                    Object_free(temp);
                    return 0;
                }
                else if(v->data.String.v[0] != '\''){
                    
                    string = (char*)malloc(sizeof(char) * (2 + v->data.String.length + 1));
                    strcpy(string, "\"");
                    strcat(string, v->data.String.v);
                    strcat(string, "\"");
                    v = Object_initString(string,
                                          strlen(string));
                    compiler(insts,
                             v,
                             vt,
                             tail_call_flag,
                             parent_func_name,
                             function_for_compilation,
                             env,
                             mt,
                             module);
                    Object_free(v);
                    free(string);
                    return 0;
                }
                return compiler(insts, v, vt, tail_call_flag, parent_func_name, function_for_compilation,env, mt, module);
            }
            else if(str_eq(tag, "def")){
                uint16_t need_to_free_var_value = 0;
                var_name = cadr(l);
                if (var_name->type == STRING
                    && (
                        str_eq(var_name->data.String.v, "quote") ||
                        str_eq(var_name->data.String.v, "quasiquote") ||
                        str_eq(var_name->data.String.v, "unquote-splice") ||
                        str_eq(var_name->data.String.v, "lambda") ||
                        str_eq(var_name->data.String.v, "fn") ||
                        str_eq(var_name->data.String.v, "def") ||
                        str_eq(var_name->data.String.v, "set!") ||
                        str_eq(var_name->data.String.v, "recur"))) {
                        printf("DEFINITION ERROR: Invalid variable name %s\n", var_name->data.String.v);
                        return 0;
                    }
                // 不支持 scheme 类似的 (def (add a b) (+ a b))
                if (cddr(l)->type == NULL_)
                    var_value = GLOBAL_NULL;
                // check (def add [a b] (+ a b)) like expression
                else if (cdddr(l) != GLOBAL_NULL){
                    // Here might exist problems
                    // need to free var_value later.
                    need_to_free_var_value = 1; 
                    var_value = cons(LAMBDA_STRING,
                                     cons(caddr(l), cdddr(l)));
                }
                else{
                    var_value = caddr(l);
                }
                var_existed = false;

                if (vt->length == 1) { // check module
                    for (i = 0; i < module->length; i++) {
                        if (str_eq(var_name->data.String.v,
                                   vt->frames[0]->var_names[module->variable_offset[i]])) {
                            printf("DEFINITION ERROR: variable: %s already defined\n", var_name->data.String.v);
                            
                            string = to_string(l);
                            printf("           EXP: %s\n", string);
                            free(string);
                            if(need_to_free_var_value) Object_free(var_value);
                            return 0;
                        }
                    }
                    // save offset to module
                    Module_addOffset(module, vt->frames[vt->length - 1]->length);
                }
                else{ // check top frame
                    frame = vt->frames[vt->length - 1];
                    for (j = frame->length - 1; j >= 0; j--) {
                        if (frame->var_names[j] == NULL) {
                            continue;
                        }
                        if (str_eq(var_name->data.String.v,
                                   frame->var_names[j])) {
                            printf("DEFINITION ERROR: variable: %s already defined\n", var_name->data.String.v);
                            string = to_string(l);
                            printf("           EXP: %s\n", string);
                            free(string);
                            if(need_to_free_var_value) Object_free(var_value);
                            return 0;
                        }
                    }
                }

                if(var_existed == false)
                    VT_push(vt, vt->length-1, var_name->data.String.v);
                if (var_value->type == PAIR &&
                    (str_eq(car(var_value)->data.String.v,
                            "lambda")
                     || str_eq(car(var_value)->data.String.v,
                               "fn"))) {
                         parent_func_name = var_name->data.String.v;
                     }
                /* 必须在 compiler 函数前保存 index. 然后 Insts_push*/
                uint16_t set_index = vt->frames[vt->length - 1]->length - 1;
                // compile value
                i = compiler(insts,
                         var_value,
                         vt,
                         tail_call_flag,
                         parent_func_name,
                         function_for_compilation,
                         env,
                         mt,
                         module);
                
                if(need_to_free_var_value) Object_free(var_value);
                
                if (i == 1) { // new required
                    LOADED_MODULES->offset = set_index;// save offset
                }
                
                // global
                if (vt->length == 1) {
                    Insts_push(insts, GLOBAL_PUSH << 12);
                    Insts_push(insts, set_index);
                    Insts_push(insts , 0x0000);
                    return 0;
                }
                // local
                else{
                    // add instruction
                    Insts_push(insts, SET_TOP << 12);
                    Insts_push(insts, set_index);
                    return 0;
                }
            }
            else if(str_eq(tag, "set!")){
                // check eg (set! x 0 12) case
                /*
                 if (cdddr(l) != GLOBAL_NULL) {
                 return compiler(insts,
                 cdr(l),
                 vt,
                 tail_call_flag,
                 parent_func_name,
                 function_for_compilation,
                 env,
                 mt);
                 }*/
                // eg (def x #[1,2,3])
                //    (set! x[0] 14)  => (x 0 3)
                //    (def z #[#[1,2] 3])
                //    (set! z[0][0] 3) => ((z 0) 0 3)
                if (cadr(l)->type == PAIR) {
                    //printf("%s\n", to_string(l));
                    //printf("%s %s\n", to_string(cadr(l)), to_string(cddr(l)));
                    Object * temp_ = list_append(cadr(l), cddr(l));
                    //printf("%s\n", to_string(temp));
                    compiler(insts,
                             temp_,
                             vt,
                             tail_call_flag,
                             parent_func_name,
                             function_for_compilation,
                             env,
                             mt,
                             module);
                    Object_free(temp_);
                    return 0;
                }
                
                // change value of a variable
                var_name = cadr(l);
                var_value = caddr(l);
                VT_find(vt, var_name->data.String.v, vt_find, module);
                if (vt_find[0] == -1) {
                    printf("SET! ERROR: undefined variable %s \n", var_name->data.String.v);
                    string = to_string(l);
                    printf("       EXP: %s\n", string);
                    free(string);

                    return 0;
                }
                else{
                    if(var_value->type == PAIR &&
                       (str_eq(car(var_value)->data.String.v,
                               "lambda")
                        || str_eq(car(var_value)->data.String.v,
                                  "fn")))
                        parent_func_name = var_name->data.String.v;
                    // compile value
                    i = compiler(insts,
                             var_value,
                             vt,
                             tail_call_flag,
                             parent_func_name,
                             function_for_compilation,
                             env,
                             mt,
                             module);
                    
                    if (i == 1) { // new loaded
                        LOADED_MODULES->offset = vt_find[1]; // save offset
                    }
                    
                    Insts_push(insts, SET << 12 | (0x0FFF & vt_find[0])); // frame_index
                    Insts_push(insts, vt_find[1]); // value index
                    return 0;
                }
            }
            /*
             *  include file like C
             *
             */
            else if (str_eq(tag, "load")){
                char abs_path[256];
                char * file_name_ptr;
                char file_name[256];
                if (cadr(l)->type == PAIR && str_eq(car(cadr(l))->data.String.v, "quote")) {
                    file_name_ptr = (char*)malloc(sizeof(char) * (256)); // max 256
                    strcpy(file_name_ptr, cadr(cadr(l))->data.String.v);
                }
                else{
                    file_name_ptr = format_string(cadr(l)->data.String.v);
                }
                uint64_t file_name_length = strlen(file_name_ptr);
                if (file_name_length > 3 &&
                    file_name_ptr[file_name_length - 1] == 'a' &&
                    file_name_ptr[file_name_length - 2] == 'w' &&
                    file_name_ptr[file_name_length - 3] == '.') {
                    strcpy(file_name, file_name_ptr);
                }
                else{
                    strcpy(file_name, file_name_ptr);
                    strcat(file_name, ".wa");
                }
// get absolute path
#ifdef WIN32
				GetFullPathName((TCHAR*)file_name, 256, (TCHAR*)abs_path, NULL); // I don't know is this correct
#else
                if(!realpath(file_name, abs_path)){
                    printf("ERROR: Failed to load %s\n", file_name);
                    return 0;
                }
#endif
				FILE * file;
                file = fopen(abs_path, "r");
                if(file == NULL){
                    printf("ERROR: Failed to load %s\n", file_name);
                    Insts_push(insts, CONST_NULL);
                    free(file_name_ptr);
                    
                    string = to_string(l);
                    printf("  EXP: %s\n", string);
                    free(string);
                    
                    return 0; // return 0 means already loaded or error
                }
                char * content;
                fseek(file, 0, SEEK_END);
                int64_t size = ftell(file);
                rewind(file);
                content = (char*)calloc(size + 1, 1);
                size_t result = fread(content,1,size,file);
                if (result != size) {
                    printf("ERROR: Failed to load %s\n", file_name);
                    free(content);
                    return 0;
                }
                fclose(file); // 不知道要不要加上这个
                
                Lexer * p;
                Object * o;
                p = lexer(content);
                if (p == NULL) {
                    printf("ERROR: parentheses () num doesn't match");
                }
                o = parser(p);
                compiler_begin(insts,
                               o,
                               vt,
                               NULL,
                               NULL,
                               0,
                               env,
                               mt,
                               module);
                free(content);
                return 0;
            }
            /*
             *  special builtin macro: load
             *  load file, return the last value in that file
             *  (def matrix (require "matrix")) ;; save test to global
             *
             */
            else if (str_eq(tag, "require")){
                if (vt->length != 1) {
                    printf("ERROR: require at invalid scope\n");
                    string = to_string(l);
                    printf("  EXP: %s\n", string);
                    free(string);
                    return 0;
                }
                else{
                    char abs_path[256];
                    char * file_name_ptr;
                    char file_name[256];
                    if (cadr(l)->type == PAIR && str_eq(car(cadr(l))->data.String.v, "quote")) {
                        file_name_ptr = (char*)malloc(sizeof(char) * (256)); // max 256
                        strcpy(file_name_ptr, cadr(cadr(l))->data.String.v);
                    }
                    else{
                        file_name_ptr = format_string(cadr(l)->data.String.v);
                    }
                    uint64_t file_name_length = strlen(file_name_ptr);
                    if (file_name_length > 3 &&
                        file_name_ptr[file_name_length - 1] == 'a' &&
                        file_name_ptr[file_name_length - 2] == 'w' &&
                        file_name_ptr[file_name_length - 3] == '.') {
                        strcpy(file_name, file_name_ptr);
                    }
                    else{
                        strcpy(file_name, file_name_ptr);
                        strcat(file_name, ".wa");
                    }
// get absolute path
#ifdef WIN32
					GetFullPathName((TCHAR*)file_name, 256, (TCHAR*)abs_path, NULL); // I don't know is this correct
#else
                    if(!realpath(file_name, abs_path)){
                        printf("ERROR: Failed to require file: %s\n", file_name);
                        return 0;
                    }
#endif                    
                    // check modules loaded or not
                    uint16_t offset = checkModuleLoaded(&LOADED_MODULES, abs_path, vt->frames[0]);
                    if (offset > 0) { // already loaded
                        Insts_push(insts, GET<<12 | 0);
                        Insts_push(insts, offset);
                        free(file_name_ptr);
                        return 0; // return 0 means already loaded
                    }
                    
                    // printf("here %s\n", abs_path);
                    /*
                     * TODO : 检查 abs_path 已经load过了
                     */
                    FILE * file;
                    file = fopen(file_name, "r");
                    if(file == NULL){
                        // check system modules
                        // at /usr/local/lib
                        // TODO : global constant SYSTEM_MODULES_PATH
                        strcpy(file_name, "/usr/local/lib/walley/");
                        if (file_name_length > 3 &&
                            file_name_ptr[file_name_length - 1] == 'a' &&
                            file_name_ptr[file_name_length - 2] == 'w' &&
                            file_name_ptr[file_name_length - 3] == '.') {
                            strcat(file_name, file_name_ptr);
                        }
                        else{
                            strcat(file_name, file_name_ptr);
                            strcat(file_name, ".wa");
                        }
                        file = fopen(file_name, "r");
                        if (file == NULL) {
                            printf("ERROR: Failed to require %s\n", file_name_ptr);
                            Insts_push(insts, CONST_NULL);
                            free(file_name_ptr);
                            
                            string = to_string(l);
                            printf("  EXP: %s\n", string);
                            free(string);
                            
                            return 0; // return 0 means already loaded or error
                        }
                    }
                    
		    //FOUND_FILE:;
                    char * content;
                    fseek(file, 0, SEEK_END);
                    int64_t size = ftell(file);
                    rewind(file);
                    content = (char*)calloc(size + 1, 1);
                    size_t result = fread(content,1,size,file);
                    if (result != size) {
                        printf("ERROR: Failed to require file: %s\n", file_name);
                        return 0;
                    }
                    fclose(file); // 不知道要不要加上这个
                    
                    
                    Lexer * p;
                    Object * o;
                    p = lexer(content);
                    if (p == NULL) {
                        printf("ERROR: parentheses () num doesn't match");
                    }
                    o = parser(p);
                /*
                    // put o inside function
                    // ((fn [] o))
                    o = cons(GLOBAL_NULL, o);
                    o = cons(LAMBDA_STRING, o);
                    o = cons(o, GLOBAL_NULL);
                    o = cons(o, GLOBAL_NULL);
                 */
                Module * new_module = Module_init();
                    compiler_begin(insts,
                                   o,
                                   vt,
                                   NULL,
                                   NULL,
                                   0,
                                   env,
                                   mt,
                                   new_module);
                Module_free(new_module);

                //LOAD_DONE:
                    free(content);
                    free(file_name_ptr);
                    return 1;
                }
            }
            // (if test conseq alter)
            else if (str_eq(tag, "if")){
                test = cadr(l);
                conseq = caddr(l);
                if(cdddr(l)->type == NULL_) alter = GLOBAL_NULL;
                else alter = cadddr(l);
                compiler(insts,
                         test,
                         vt,
                         false,
                         parent_func_name,
                         function_for_compilation,
                         env,
                         mt,
                         module);
                // push test, but now we don't know jump steps
                Insts_push(insts, TEST << 12); // jump over consequence
                index1 = insts->length;
                Insts_push(insts, 0x0000); // jump steps
                
                //printf("\n@ %ld\n", insts->length);
                //parser_debug(conseq);
                // compiler_begin ...
                compiler_begin(insts,
                               cons(conseq, GLOBAL_NULL),
                               vt,
                               parent_func_name,
                               function_for_compilation,
                               false, // cannot eval
                               env,
                               mt,
                               module);
                //printf("\n@ %ld\n", insts->length);
                
                index2 = insts->length;
                Insts_push(insts, JMP<<12); // jmp
                Insts_push(insts, 0x0000); // jump over alternative
                Insts_push(insts, 0x0000);
                jump_steps = index2 - index1 + 4;
                Insts_set(insts, index1, jump_steps);
                
                // compiler begin
                compiler_begin(insts,
                               cons(alter, GLOBAL_NULL),
                               vt,
                               parent_func_name,
                               function_for_compilation,
                               /*vt->length == 1 ? true :*/false,
                               env,
                               mt,
                               module);
                
                index3 = insts->length;
                jump_steps = index3 - index2;
                Insts_set(insts, index2 + 1, (0xFFFF0000 & jump_steps) >> 16);
                Insts_set(insts, index2 + 2, (0x0000FFFF & jump_steps));
                
                // run if if necessary
                /*
                 // cannot set acc,
                 // so this way doesn't work
                 if (vt->length == 1) {
                 VM(insts->array,
                 insts->start_pc,
                 insts->length,
                 env);
                 insts->start_pc = insts->length;
                 }
                 */
                return 0;
            }
            else if (str_eq(tag, "begin")){
                // compiler begin
                compiler_begin(insts,
                               cdr(l),
                               vt,
                               parent_func_name,
                               function_for_compilation,
                               false,
                               env,
                               mt,
                               module);
                return 0;
            }
            else if (str_eq(tag, "lambda") || str_eq(tag, "fn")){ // I am too lazy
                Object * params = cadr(l); // get parameters
                int32_t variadic_place = -1; // variadic place
                int32_t counter = 0; // count of parameter num
                Variable_Table * vt_ = VT_copy(vt); // new variable table
                MacroTable * mt_ = MT_copy(mt); // new macro table
                VT_add_new_empty_frame(vt_); // we add a new frame
                MT_add_new_empty_frame(mt_); // we add a new frame
                
                //macros_.push([]); // add new frame
                //env_.push([]); // 必须加上这个， 要不然((lambda () (defmacro square ([x] `[* ~x ~x])) (square 12))) macro 会有错
                while (true) {
                    if (params == GLOBAL_NULL) {
                        break;
                    }
                    if (car(params)->type != STRING) {
                        printf("ERROR: Invalid Function Parameter type\n");
                        string = to_string(l);
                        printf("  EXP: %s\n", string);
                        free(string);
                        return 0;
                    }
                    if (str_eq(car(params)->data.String.v,
                               ".")) { // variadic
                        variadic_place = counter;
                        VT_push(vt_, vt_->length - 1, cadr(params)->data.String.v);
                        counter += 1; // means no parameters requirement
                        break;
                    }
                    VT_push(vt_,
                            vt_->length - 1,
                            car(params)->data.String.v);
                    counter++;
                    params = cdr(params);
                }
                
                // make lambda
                Insts_push(insts,
                           (MAKELAMBDA << 12)
                           | (counter << 6)
                           | (variadic_place == -1? 0 : variadic_place << 1)
                           | (variadic_place == -1 ? 0 : 1));
                index1 = insts->length;
                Insts_push(insts, 0x0000); // steps that needed to jump over lambda
                Insts_push(insts, 0x0000); // used to save frame-length
                start_pc = insts->length; // get start_pc
                
                // set function_for_compilation
				Lambda_for_Compilation * function_ = (Lambda_for_Compilation*)malloc(sizeof(Lambda_for_Compilation));
                function_->start_pc = start_pc;
                function_->param_num = counter;
                function_->variadic_place = variadic_place;
                function_->vt = vt_;
                function_->is_tail_call = 0; // for tail call
                
                // compile body
                // 关于这里 compiler_begin 不用 free function_里面的vt_
                // 因为 vt_ 已经被 free 掉
                compiler_begin(insts,
                               cddr(l),
                               vt_,
                               parent_func_name,
                               function_,
                               false,
                               env,
                               mt_,
                               module);
                // return
                Insts_push(insts, RETURN << 12);
                index2 = insts->length;
                insts->array[index1] = index2 - index1 + 1; // set jump steps
                
                // save frame length
                if (variadic_place != -1) { // variadic
                    insts->array[index1 + 1] = 64; // 64 maximum
                }
                else{
                    insts->array[index1 + 1] = vt_->frames[vt_->length - 1] -> length; // save frame length
                }
                
                // 这里出错了, 因该只用free 最top的
                // VT_free(vt_); // free vt_;
                // free(vt_);
                for (i = 0; i < vt_->frames[vt_->length-1]->length; i++) {
                    // because I might set it to NULL when doing tail call
                    if (vt_->frames[vt_->length - 1]->var_names[i] != NULL) {
                        free(vt_->frames[vt_->length - 1]->var_names[i]);
                        vt_->frames[vt_->length - 1]->var_names[i] = NULL;
                    }
                }
                free(vt_->frames[vt_->length - 1]->var_names);
                vt_->frames[vt_->length - 1]->var_names = NULL;
                free(vt_->frames[vt_->length - 1]);
                vt_->frames[vt_->length - 1] = NULL;
                free(vt_);
                
                // free macro table
                MacroTableFrame * top_frame = mt_->frames[mt_->length - 1];
                for (i = 0; i < top_frame->length; i++) {
                    Macro_free(top_frame->array[i]);
                    top_frame->array[i] = NULL;
                }
                free(top_frame->array);
                top_frame->array = NULL;
                free(top_frame);
                free(mt_);
                
                vt_ = NULL;
                mt_ = NULL;
                
                free(function_); // free lambda for compilation
                function_ = NULL;
                
                return 0;
            }
            /*
             (defmacro macro_name 
                       var0 pattern0 
                       var1 pattern1 ...)
             */
            else if (str_eq(tag, "defmacro")){
                var_name = cadr(l);
                Object * clauses = cddr(l);
                MacroTableFrame * frame = mt->frames[mt->length-1]; // get op frame
                // length = frame->length;
                for (i = frame->length - 1; i >= 0; i--) {
                    if (str_eq(var_name->data.String.v, frame->array[i]->macro_name)) { // already existed
                        
                        free(frame->array[i]->macro_name);
                        frame->array[i]->clauses->use_count--;
                        Object_free(frame->array[i]->clauses);
                        
                        clauses->use_count+=1; // 必须+1
                        frame->array[i] = Macro_init(var_name->data.String.v, (clauses), VT_copy(vt));
                        return 0;
                    }
                }
                // is not defined
                // resize if size is not enough
                if (frame->length == frame->size) {
                    frame->size*=2;
                    frame->array = (Macro**)realloc(frame->array, frame->size);
                }
                
                clauses->use_count+=1; // 必须+1
                frame->array[frame->length] = Macro_init(var_name->data.String.v, (clauses), VT_copy(vt));
                frame->length+=1;
                return 0;
            }
            /*
             * export macro to upper level.
             * eg (defn test [] 
             *        (defm square [x] `(* ~x ~x))
             *        (export-macro square))       ;; export to upper level
             *    (square 12) ;; => 144
             (def test (fn [] (defm square [x] `(* ~x ~x)) (export-macro square)))(square 12) ;; => 144
             */
            /*
            else if (str_eq(tag, "export-macro")){ // move macro to upper level
                if (mt->length <= 1) {
                    return 0;
                }
                MacroTableFrame * mt_top_frame = mt->frames[mt->length - 1];
                Macro * new_macro = NULL;
                int8_t found_macro = 0;
                // find macro
                for (i = mt_top_frame->length - 1; i >= 0; i--) {
                    if (str_eq(mt_top_frame->array[i]->macro_name, cadr(l)->data.String.v)) { // find macro
                        new_macro = Macro_init(mt_top_frame->array[i]->macro_name, mt_top_frame->array[i]->clauses,
                            mt_top_frame->array[i]->vt);
                        found_macro = 1;
                        break;
                    }
                }
                if (!found_macro) {
                    printf("MACRO EXPORT ERROR: macro %s does not exist\n", cadr(l)->data.String.v);
                    return 0;
                }
                else{ // found macro
                    //
                    //为了保险加上 ()
                    //
                    Insts_push(insts, CONST_NULL);
                    MacroTableFrame * upper_level_frame = mt->frames[mt->length - 2]; // get upper level frame
                    for (i = upper_level_frame->length - 1; i >= 0; i--) {
                        if (str_eq(cadr(l)->data.String.v, upper_level_frame->array[i]->macro_name)) { // already existed
                            // free old macro
                            Macro * old_macro = upper_level_frame->array[i];
                            free(old_macro->macro_name);
                            old_macro->clauses->use_count--;
                            Object_free(old_macro->clauses);
                            free(old_macro);
                            // set new macro
                            upper_level_frame->array[i] = new_macro;
                            return 0;
                        }
                    }
                    // is not defined
                    // resize if size is not enough
                    if (upper_level_frame->length == upper_level_frame->size) {
                        upper_level_frame->size*=2;
                        upper_level_frame->array = realloc(upper_level_frame->array, upper_level_frame->size);
                    }
                    upper_level_frame->array[upper_level_frame->length] = new_macro;
                    upper_level_frame->length+=1;
                    return 0;
                }
            }
            */
            // call function
            else{
                // check is macro
                if (car(l)->type == STRING) {
                    MT_find(mt, car(l)->data.String.v, vt_find);
                    if (vt_find[0] != -1) { // find macro
                        Object * expand = macro_expand_for_compilation(mt->frames[vt_find[0]]->array[vt_find[1]],
                                                                       cdr(l),
                                                                       mt,
                                                                       env,
                                                                       insts,
                                                                       module);
                        compiler(insts,
                                 expand,
                                 vt,
                                 tail_call_flag,
                                 parent_func_name,
                                 function_for_compilation,
                                 env,
                                 mt,
                                 module);
                        Object_free(expand);
                        return 0;
                    }
                }
                // tail call function
                if(tail_call_flag){
                    // so no new frame
                    int32_t start_index = vt->frames[vt->length - 1]->length;
                    int32_t track_index = start_index;
                    // compile parameters
                    int32_t param_num = 0; // param num
                    Object * p = cdr(l);
                    Object * params[64]; // 最多存 64 个 params
                    int32_t count_params = 0; // count param num
                    while (p != GLOBAL_NULL) {
                        params[param_num] = car(p);
                        param_num++;
                        p = cdr(p);
                    }
                    
                    // only one parameters
                    // eg (def test (lambda [i] (if (= i 0) 0 (test (- i 1)))))
                    // 但是没想到这样反而慢了。。。
                    
                    if (param_num == 1) {
                        compiler(insts,
                                 params[0],
                                 vt,
                                 false,
                                 parent_func_name,
                                 function_for_compilation,
                                 env,
                                 mt,
                                 module); // compile that one parameter
                        
                        // set tp current frame
                        Insts_push(insts, (SET << 12) | (vt->length - 1)); // frame index
                        Insts_push(insts, 0x0000FFFF & 0x0000); // value index, which is 0
                        goto tail_call_function_compilation_jmp_back;
                    }
                    /*compile parameters*/
                    for (i = 0; i < param_num; i++) {
                        if(i == function_for_compilation->variadic_place){ // variadic place
                            count_params++;
                            p = GLOBAL_NULL;
                            j = param_num - 1;
                            for (; j >= i; j--) {
                                p = cons(CONS_STRING,
                                         cons(params[j],
                                              cons(p, GLOBAL_NULL)));
                            }
                            compiler(insts,
                                     p,
                                     vt,
                                     false,
                                     parent_func_name,
                                     function_for_compilation,
                                     env,
                                     mt,
                                     module); // each argument is not tail call
                            
                            Object_free(p);
                            
                            // set tp current frame
                            //Insts_push(insts, (SET << 12) | (vt->length - 1)); // frame index
                            // 这里不能用 SET_OP, 因为stack的size会一直增长。。。
                            // Insts_push(insts, (SET_TOP << 12)); // frame index
                            // Insts_push(insts, 0x0000FFFF & track_index);
                            Insts_push(insts, TAIL_CALL_PUSH << 12 | track_index);
                            // value index
                        }
                        else{
                            count_params++;
                            compiler(insts,
                                     params[i],
                                     vt,
                                     false,
                                     parent_func_name,
                                     function_for_compilation,
                                     env,
                                     mt,
                                     module); // this argument is not tail call
                            // set to current frame
                            //Insts_push(insts, (SET << 12) | (vt->length - 1)); // frame index
                            // 这里不能用 SET_OP, 因为stack的size会一直增长。。。
                            // Insts_push(insts, (SET_TOP << 12)); // frame index
                            // Insts_push(insts, 0x0000FFFF & track_index);
                            
                            Insts_push(insts, TAIL_CALL_PUSH << 12 | track_index);
                            // value index
                        }
                        track_index++;
                    }
                    // set tail_call flag
                    if (!(function_for_compilation->is_tail_call)) {
                        // change vt length
                        for (i = 0; i < count_params; i++) {
                            VT_push(function_for_compilation->vt,
                                    function_for_compilation->vt->length - 1,
                                    NULL);
                        }
                        if (function_for_compilation->variadic_place == -1 && i < function_for_compilation->param_num) {
                            for (; i < function_for_compilation->param_num; i++) {
                                VT_push(function_for_compilation->vt,
                                        function_for_compilation->vt->length - 1,
                                        NULL);
                            }
                        }
                    }
                    
                    // move parameters
                    for (i = 0; i < count_params; i++) {
                        // get value
                        Insts_push(insts, (GET << 12) | (vt->length - 1)); //frame index
                        Insts_push(insts, start_index + i); // value index
                        // move to target index
                        Insts_push(insts, (SET << 12) | (vt->length - 1)); // frame index
                        Insts_push(insts, i); // value index 不再加2是因为不再存env 和 pc
                        //Insts_push(insts, (TAIL_CALL_PUSH << 12) | i );
                    }
                    if (function_for_compilation->variadic_place == -1 && i < function_for_compilation->param_num) {
                        for (; i < function_for_compilation->param_num; i++) {
                            Insts_push(insts, CONST_NULL);
                            // move to target index
                            Insts_push(insts, (SET << 12) | (vt->length - 1)); // frame index
                            Insts_push(insts, i); // value index 不再加2是因为不再存env 和 pc
                            //Insts_push(insts, (TAIL_CALL_PUSH << 12) | i );
                        }
                    }
                tail_call_function_compilation_jmp_back:
                    // jump back
                    start_pc = function_for_compilation->start_pc;
                    Insts_push(insts, JMP << 12);
					jump_steps = (uint64_t)(-(int64_t)(insts->length - start_pc) + 1); // jump steps
                    Insts_push(insts, (0xFFFF0000 & jump_steps) >> 16);
                    Insts_push(insts, 0x0000FFFF & jump_steps);
                    return 0;
                }
                // not tail call
                else{
                    // compile function first
                    compiler(insts,
                             car(l),
                             vt,
                             false,
                             parent_func_name,
                             function_for_compilation,
                             env,
                             mt,
                             module); // compile lambda, save to accumulator
                    Insts_push(insts, NEWFRAME << 12); // create new frame
                    // compile paremeters
                    int32_t param_num = 0;
                    Object * p = cdr(l);
                    Object * params[64]; // 最多存 64 个 params
                    while (p != GLOBAL_NULL) {
                        params[param_num] = car(p);
                        param_num++;
                        p = cdr(p);
                    }
                    /*
                     * TODO : check param name valid? here
                     *
                     */
                    // compile params from left to right
                    for (i = 0; i < param_num; i++) {
                        compiler(insts,
                                 params[i],
                                 vt,
                                 false,
                                 parent_func_name,
                                 function_for_compilation,
                                 env,
                                 mt,
                                 module);// each argument is not tail call
                        Insts_push(insts, PUSH_ARG << 12);
                    }
                    Insts_push(insts, (CALL << 12) | (0x0FFF & param_num));
                    return 0;
                }
            }
        default:
            
            printf("ERROR: Invalid Data\n");
            string = to_string(l);
            printf("  EXP: %s\n", string);
            free(string);
            return 0;
            break;
    }
}
/*
 compile a series of expression
 */
Object * compiler_begin(Instructions * insts,
                        Object * l,
                        Variable_Table * vt,
                        char * parent_func_name,
                        Lambda_for_Compilation * function_for_compilation,
                        int32_t eval_flag,
                        Environment * env,
                        MacroTable * mt,
                        Module * module){
    
    Object * acc = GLOBAL_NULL;
    Object * l_ = l; // make a copy of l, so that we can free it later
    while (l != GLOBAL_NULL) {
        if (cdr(l) == GLOBAL_NULL    // the last statement
            && car(l)->type == PAIR
            && ( (parent_func_name != NULL
                  && str_eq(car(car(l))->data.String.v, parent_func_name))
                || str_eq(car(car(l))->data.String.v, "recur"))
            ) {
            // tail call
            compiler(insts,
                     car(l),
                     vt,
                     1, // tail call
                     NULL,
                     function_for_compilation,
                     env,
                     mt,
                     module);
        }
        else{
            compiler(insts,
                     car(l),
                     vt,
                     0,
                     parent_func_name,
                     function_for_compilation,
                     env,
                     mt,
                     module);
        }
        l = cdr(l);
        
        if (eval_flag) {
#if COMPILER_DEBUG
            printInstructions(insts);
#endif
            acc = VM(insts,
                     insts->start_pc,
                     insts->length,
                     env,
                     vt,
                     mt,
                     module,
                     GLOBAL_NULL,
                     NULL); // run vm
            insts->start_pc = insts->length; // update start pc
            
#if COMPILER_DEBUG
            printf("\n### COMPILER_BEGIN VM ####");
            printf("\nGLOBAL FRAME => length %d", env->frames[0]->length);
#endif
        }
    }
    
    Object_free(l_); // free parser
    
    return acc;
}


#endif
