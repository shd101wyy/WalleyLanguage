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
void compiler(Instructions * insts,
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
           MacroTable * mt);

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
    macro_match
    return length of var_names
   (defmacro test [(x) x] [(#hi x) y] [(x y) x])

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
             || (a != GLOBAL_NULL && b == GLOBAL_NULL)){
        return 0; // doesn't match
    }
    else if(car(a)->type == PAIR && car(b)->type == PAIR){
        int32_t match = macro_match(car(a), car(b), var_names, var_values, count);
        if (!match) return 0; // doesn't match
        return macro_match(cdr(a),
                           cdr(b),
                           var_names,
                           var_values,
                           match);
    }
    else if(car(a)->type == PAIR && car(b)->type != PAIR)
        return 0;
    else{
        if (car(a)->type == STRING && car(a)->data.String.v[0] == '#') {
            // constant
            if(car(b)->type != STRING) return 0; // doesn't match
            if (strcmp( &(*((char*)car(a)->data.String.v + 1)), car(b)->data.String.v) == 0) {
                return macro_match(cdr(a),
                                   cdr(b),
                                   var_names,
                                   var_values,
                                   count);
            }
            else{
                return 0;
            }
        }
        if (car(a)->type == STRING &&
            str_eq(car(a)->data.String.v, ".")) {
            // 这里 var_name 前加 空格
            char * temp = malloc(sizeof(char) * (strlen(cadr(a)->data.String.v) + 2));
            strcpy(temp, " ");
            strcat(temp, cadr(a)->data.String.v);
            var_names[count] = temp;// cadr(a)->data.String.v; // save var name
            var_values[count] = b; // save var value
            count++;
            return count;
        }
        var_names[count] = car(a)->data.String.v; // save var_name
        var_values[count] = car(b); // save var_value
        count++;
        return macro_match(cdr(a),
                           cdr(b),
                           var_names,
                           var_values,
                           count);
    }
}

Object * macro_expansion_replacement(Object * expanded_value,
                                Variable_Table * vt,
                                 int32_t is_head){
    if (expanded_value == GLOBAL_NULL) {
        return GLOBAL_NULL;
    }
    if (expanded_value->type != PAIR) {
        return expanded_value;
    }
    Object * v = car(expanded_value);
    if (v->type == PAIR) {
        return cons(macro_expansion_replacement(v, vt, true),
                    macro_expansion_replacement(cdr(expanded_value),
                                                vt, false));
    }
    else{
        if (v->type == STRING && is_head) { // check in vt
            if (str_eq(v->data.String.v, "quote")
                ||str_eq(v->data.String.v, "quasiquote")) {// is quote function, so no replacement
                return expanded_value;
            }
            if (v->data.String.v[0] == '!') { // means not replace
                char buffer[v->data.String.length];
                strcpy(buffer, &v->data.String.v[1]);
                // printf("BUFFER! %s\n", buffer);
                return cons(Object_initString(buffer, strlen(buffer)),
                            macro_expansion_replacement(cdr(expanded_value), vt, false));
            }
            int32_t find[2];
            /*
             todo: change this VT_find later
             */
            VT_find(vt, v->data.String.v, find, NULL);
            if (find[0] != -1) { // find
                return cons(cons(Object_initInteger(0),
                                 cons(Object_initInteger(find[0]),
                                      cons(Object_initInteger(find[1]),
                                           GLOBAL_NULL))),
                            macro_expansion_replacement(cdr(expanded_value),
                                vt, false));
            }
            else
                return cons(v, macro_expansion_replacement(cdr(expanded_value), vt, false));
        }
        else if(v->type == STRING ||
                v->type == INTEGER ||
                v->type == DOUBLE ||
                v->type == NULL_)
            return cons(v, macro_expansion_replacement(cdr(expanded_value), vt, false));
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
    (defmacro square ([x] `(* ~x ~x)))
    (square 12) => (* 12 12)
 */
Object * macro_expand_for_compilation(Macro * macro, Object * exps, MacroTable * mt, Environment * global_env, Instructions * insts, Module *module){
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
        match = macro_match(car(car(clauses)),
                            exps,
                            var_names,
                            var_values,
                            0);
        if (match || (exps == GLOBAL_NULL && car(car(clauses)) == GLOBAL_NULL)) {
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
            new_vt_top_frame = malloc(sizeof(Variable_Table_Frame));
            // new_vt_top_frame->use_count = 1; // 后面要被使用
            new_vt_top_frame->var_names = malloc(sizeof(char*)*match);
            new_vt_top_frame->length = match; // set length directly
            
            // init vt
            new_vt = malloc(sizeof(Variable_Table));
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
            new_env_top_frame->use_count += 1 ;
            
            // add var name to top frame of new_vt;
            for (i = 0; i < match; i++) {
                
                
                if (var_names[i][0] == ' ') {
                    char * a = malloc(sizeof(char) * (strlen(var_names[i])));
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
                          cons(cadr(car(clauses)), GLOBAL_NULL)
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
            expanded_value = VM(insts, insts_length, insts->length, new_env, NULL, NULL);
            
           
#if MACRO_DEBUG
            printf("\n");
            printInstructions(insts);
            printf("\nFinish Expansion\n");
            // 这里结束可以用于 (macroexpand)
            parser_debug(expanded_value);
            //exit(0);
#endif
            // printf("%s\n", to_string(expanded_value));
            // restore start-pc and length
            insts->start_pc = start_pc;
            insts->length = insts_length;

            // 一改还得free其他的, var_names和var_values不用free
            // 因为 var_names 是保存在 macro 里面的
            //     var_values 会随着 parser 而free掉
            // free new_vt
            free(new_vt);
            /*
            for (i = 0; i < new_vt_top_frame->length; i++) {
                free(new_vt_top_frame->var_names[i]);
            }*/
            free(new_vt_top_frame->var_names);
            free(new_vt_top_frame);
            
            // free new_env
            free(new_env);
            new_env_top_frame->use_count-=1;
            EF_free(new_env_top_frame);
            
            //return expanded_value;
            
            // 假设运行完了得到了 expanded_value
            // 根据 macro->vt 替换首项
            expanded_value_after_replacement = macro_expansion_replacement(expanded_value, macro->vt, true);
            Object_free(expanded_value);
            return expanded_value_after_replacement;
             
        }
        clauses = cdr(clauses);
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
void compiler(Instructions * insts,
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
    switch (l->type) {
        case NULL_:
            Insts_push(insts, CONST_NULL); // push null;
            return;
        case INTEGER: // integer
            int_ = l->data.Integer.v;
            
            // if int_ is within [0 250), then load from constant pool
            if (int_ >= 0 && int_ < 250) {
                // load from constant pool
                Insts_push(insts, CONST_LOAD);
                Insts_push(insts, int_);
                return;
            }
            Insts_push(insts, CONST_INTEGER);
            Insts_push(insts, (0xFFFF000000000000ULL & (uint64_t)int_) >> 48);
            Insts_push(insts, (0x0000FFFF00000000ULL & (uint64_t)int_) >> 32);
            Insts_push(insts, (0x00000000FFFF0000 & int_) >> 16);
            Insts_push(insts, 0xFFFF & int_);
            return;
        case DOUBLE:
            double_ = l->data.Double.v;
            uint64_t * unsigned_int_ = (uint64_t *)&double_; // get hex
            Insts_push(insts, CONST_FLOAT);
            Insts_push(insts, (0xFFFF000000000000ULL & (uint64_t)(*unsigned_int_)) >> 48);
            Insts_push(insts, (0x0000FFFF00000000ULL & (uint64_t)(*unsigned_int_)) >> 32);
            Insts_push(insts, (0x00000000FFFF0000 & (*unsigned_int_)) >> 16);
            Insts_push(insts, 0xFFFF & (*unsigned_int_));
            return;
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
                    return;
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
                
                Insts_push(CONSTANT_TABLE_INSTRUCTIONS, CONST_STRING);
                Insts_push(CONSTANT_TABLE_INSTRUCTIONS, length);
                find_end = false;
                for (i = 0; i < length; i = i + 2) {
                    if(i + 1 == length){
                        Insts_push(CONSTANT_TABLE_INSTRUCTIONS, (s[i] << 8) & 0xFF00);
                        find_end = true;
                        break;
                    }
                    else {
                        Insts_push(CONSTANT_TABLE_INSTRUCTIONS, (s[i] << 8) | s[i+1]);
                    }
                }
                if(find_end == false){
                    Insts_push(CONSTANT_TABLE_INSTRUCTIONS, 0x0000); // add end
                }
                free(s);
                return;
            }
            else{
                VT_find(vt, l->data.String.v, vt_find, module);
                if(vt_find[0] == -1){
                    // variable doesn't exist
                    printf("ERROR: undefined variable %s\n", l->data.String.v);
                    return;
                }
                Insts_push(insts, GET<<12 | vt_find[0]); // frame index
                Insts_push(insts, vt_find[1]); // value index
                return;
            }

        case PAIR:
            if(car(l)->type == INTEGER && car(l)->data.Integer.v == 0){
                // set
                Insts_push(insts, (GET << 12) | (0x0FFF & cadr(l)->data.Integer.v));
                Insts_push(insts, caddr(l)->data.Integer.v);
                return;
            }
            tag = car(l)->data.String.v;
            if(str_eq(tag, "quote")){
                v = cadr(l);
                // check integer float null
                if(v->type == NULL_ || v->type == INTEGER || v->type == DOUBLE){
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
                    return;
                }
                else if(v->data.String.v[0] != '\''){
                    string = malloc(sizeof(char) * (2 + v->data.String.length + 1));
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
                    return;
                }
                return compiler(insts, v, vt, tail_call_flag, parent_func_name, function_for_compilation,env,mt,module);
            }
            else if(str_eq(tag, "quasiquote")){
                v = cadr(l);
                // check integer float null
                if(v->type == NULL_ || v->type == INTEGER || v->type == DOUBLE){
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
                    return;
                }
                else if(v->data.String.v[0] != '\''){
                    
                    string = malloc(sizeof(char) * (2 + v->data.String.length + 1));
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
                    return;
                }
                return compiler(insts, v, vt, tail_call_flag, parent_func_name, function_for_compilation,env, mt, module);
            }
            else if(str_eq(tag, "def")){
                var_name = cadr(l);
                if (var_name->type == STRING
                    && (
                        str_eq(var_name->data.String.v, "quote") ||
                        str_eq(var_name->data.String.v, "quasiquote") ||
                        str_eq(var_name->data.String.v, "unquote-splice") ||
                        str_eq(var_name->data.String.v, "lambda") ||
                        str_eq(var_name->data.String.v, "def") ||
                        str_eq(var_name->data.String.v, "set!"))) {
                    printf("ERROR: Invalid variable name %s\n", var_name->data.String.v);
                    return;
                }
                // 不支持 scheme 类似的 (def (add a b) (+ a b))
                if (cddr(l)->type == NULL_)
                    var_value = GLOBAL_NULL;
                else
                    var_value = caddr(l);
                
                if(vt->length > 1){ // local def
                    var_existed = false;
                    // var_index = -1;
                    frame = vt->frames[vt->length - 1];
                    for (j = frame->length - 1; j >= 0; j--) {
                        if (frame->var_names[j] == NULL) {
                            continue;
                        }
                        if (str_eq(var_name->data.String.v,
                                   frame->var_names[j])) {
                            printf("ERROR: variable: %s already defined\n", var_name->data.String.v);
                            return;
                        }
                    }
                    if(var_existed == false)
                        VT_push(vt, vt->length-1, var_name->data.String.v);
                    if (var_value->type == PAIR &&
                        str_eq(car(var_value)->data.String.v,
                               "lambda")) {
                            parent_func_name = var_name->data.String.v;
                        }
                    // compile value
                    compiler(insts,
                             var_value,
                             vt,
                             tail_call_flag,
                             parent_func_name,
                             function_for_compilation,
                             env,
                             mt,
                             module);
                    // add instruction
                    Insts_push(insts, SET_TOP << 12);
                    Insts_push(insts, vt->frames[vt->length - 1]->length - 1);
                    return;
                }
                /*
                 * todo: 定义 不存在的module. eg (def 不存在的module/x 20)
                 *
                 */
                else{ // global def
                    /*
                     *
                     * todo: 支持 (def 已存在module/x 12)
                     *
                     */
                    // 检查当前的模块
                    // check current module to see whether that variable exists
                    var_existed = false;
                    Variable_Table_Frame * f = vt->frames[0]; // get global frame
                    for (i = 0; i < *(module->length); i++) { // 查看变量是否在当前模块中存在
                        if (str_eq(var_name->data.String.v, f->var_names[module->vtf_offset[i]])) {
                            var_existed = true;
                            break;
                        }
                    }
                    if (var_existed) { // variable already defined in that module
                        printf("ERROR: variable: %s already defined in module: %s\n", var_name->data.String.v, module->module_abs_path);
                        return;

                    }
                    else{ // add that variable
                        Module_pushVarOffset(module, f->length); // save offset to module
                        VT_push(vt, vt->length-1, var_name->data.String.v); // push to vt
                        
                        if (var_value->type == PAIR &&
                            str_eq(car(var_value)->data.String.v,
                                   "lambda")) {
                                parent_func_name = var_name->data.String.v;
                            }
                        // compile value
                        compiler(insts,
                                 var_value,
                                 vt,
                                 tail_call_flag,
                                 parent_func_name,
                                 function_for_compilation,
                                 env,
                                 mt,
                                 module);
                        // add instruction
                        Insts_push(insts, SET_TOP << 12);
                        Insts_push(insts, vt->frames[vt->length - 1]->length - 1);
                        return;
                    }
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
                    return;
                }
                
                // change value of a variable
                var_name = cadr(l);
                var_value = caddr(l);
                VT_find(vt, var_name->data.String.v, vt_find, module);
                if (vt_find[0] == -1) {
                    printf("ERROR: undefined variable %s \n", var_name->data.String.v);
                    return;
                }
                else{
                    if(var_value->type == PAIR &&
                       str_eq(car(var_value)->data.String.v,
                              "lambda"))
                        parent_func_name = var_name->data.String.v;
                    // compile value
                    compiler(insts,
                             var_value,
                             vt,
                             tail_call_flag,
                             parent_func_name,
                             function_for_compilation,
                             env,
                             mt,
                             module);
                    Insts_push(insts, SET << 12 | (0x0FFF & vt_find[0])); // frame_index
                    
                    Insts_push(insts, vt_find[1]); // value index
                    return;
                }
            }
            /*
             *  special builtin macro: load
             *  (load "list") ; will load list.wa file with module NULL
             *  (load "list" as "list") load file with module: list
             */
            else if (str_eq(tag, "load")){
                if (vt->length != 1) {
                    printf("ERROR: load invalid place\n");
                    return;
                }
                else{
                    char * file_name = NULL;
                    char abs_path[256];
                    Module * load_module;
                    char module_loaded_flag = 0; // check module loaded or not
                    char * content = NULL;
                    char module_as_name_[256];// module name max length = 255

                    if (cadr(l)->type == PAIR && str_eq(car(cadr(l))->data.String.v, "quote")) {
                        file_name = malloc(sizeof(char) * (256)); // max 256
                        strcpy(file_name, cadr(cadr(l))->data.String.v);
                    }
                    else{
                        file_name = format_string(cadr(l)->data.String.v);
                    }
                    uint64_t file_name_length = strlen(file_name);
                    FILE *file;
                    // check . existed
                    if (file_name_length > 3 &&
                        file_name[file_name_length - 1] == 'a' &&
                        file_name[file_name_length - 2] == 'w' &&
                        file_name[file_name_length - 3] == '.') {
                        // read content from file
                        file = fopen(file_name,"r");
                        
                        if(file == NULL){
                            printf("ERROR: Failed to load %s\n", file_name);
                            goto LOAD_DONE; // fail to read
                        }
                        
                        // get absolute path
                        realpath(file_name, abs_path);
                    }
                    else{
                        char temp_file_name[file_name_length + 5];
                        strcpy(temp_file_name, file_name);
                        strcat(temp_file_name, ".wa");
                        // read content from file
                        file = fopen(temp_file_name, "r");
                        
                        if(file == NULL){
                            printf("ERROR: Failed to load %s\n", file_name);
                            goto LOAD_DONE; // fail to read
                        }
                        
                        // get absolute path
                        realpath(temp_file_name, abs_path);
                    }
                    
                    /*
                     * check module already loaded
                     *
                     */
                    load_module = Module_loaded(GLOBAL_MODULE, abs_path); // check from global module
                    if (load_module) {
                        // printf("Module Already Loaded %s\n", abs_path);
                        module_loaded_flag = 1; // already loaded
                    }
                    else{
                        // printf("Module not loaded %s\n", abs_path);
                        module_loaded_flag = 0; // not loaded
                    }
                    
                    // check namespace
                    if (cddr(l) != GLOBAL_NULL) {
                        // (load 'test as 'test)
                        if (str_eq(caddr(l)->data.String.v, "as")) {
                            // get module name name
                            if (cadddr(l)->type == PAIR && str_eq(car(cadddr(l))->data.String.v, "quote")) {
                                strcpy(module_as_name_, cadr(cadddr(l))->data.String.v);
                            }
                            else{
                                string = format_string(cadddr(l)->data.String.v);
                                strcpy(module_as_name_, string);
                                free(string);
                            }
                            if (module_loaded_flag == 0) { // not loaded
                                load_module = Module_init(module_as_name_); // init load module
                            }
                            else{ // already loaded
                                load_module = Module_copyWithNewAsName(load_module, module_as_name_);
                                strcpy(load_module->module_abs_path, abs_path);
                                Module_appendChild(module, load_module); // append to current module.
                                goto LOAD_DONE;
                            }
                            
                            goto LOAD_MODULE;
                        }
                        else if (str_eq(caddr(l)->data.String.v, "all")){
                            // read content
                            fseek(file, 0, SEEK_END);
                            int64_t size = ftell(file);
                            rewind(file);
                            content = calloc(size + 1, 1);
                            fread(content,1,size,file);
                            fclose(file); // 不知道要不要加上这个
                            
                            Lexer * p;
                            Object * o;
                            p = lexer(content);
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
                        }
                        else{
                            printf("ERROR: load invalid args\n");
                            printf("%s\n", to_string(l));
                            free(file_name);
                            free(content);
                            return;
                        }
                    }
                    else{
                        // module as name is file_name
                        /*
                         * todo: 这里以后得改， 不能是file_name， 而应该是 file_name 最后"/" 之后的
                         */
                        if (module_loaded_flag == 0) { // not loaded
                            // strcpy(module_as_name_, file_name);
                            load_module = Module_init(file_name); // init load module
                        }
                        else{ // already loaded
                            load_module = Module_copyWithNewAsName(load_module, file_name);
                            strcpy(load_module->module_abs_path, abs_path);
                            Module_appendChild(module, load_module); // append to current module.
                            goto LOAD_DONE;
                        }

                    LOAD_MODULE:
                        // set abs_path
                        strcpy(load_module->module_abs_path, abs_path);
                        
                        // read content
                        fseek(file, 0, SEEK_END);
                        int64_t size = ftell(file);
                        rewind(file);
                        content = calloc(size + 1, 1);
                        fread(content,1,size,file);
                        fclose(file); // 不知道要不要加上这个
                        
                        
                        Lexer * p;
                        Object * o;
                        p = lexer(content);
                        o = parser(p);

                        // 必须放在 compiler_begin 之前
                        Module_appendChild(module, load_module); // add to module children list

                        compiler_begin(insts,
                                       o,
                                       vt,
                                       NULL,
                                       NULL,
                                       0,
                                       env,
                                       mt,
                                       load_module);
                    }
                    
                LOAD_DONE:
                    if (file_name) {
                        free(file_name);
                    }
                    if (content) {
                        free(content);
                    }
                    return;
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
                return;
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
                return;
            }
            else if (str_eq(tag, "let")){
                Object * chunk = cadr(l);
                //Object * body; => cddr(l)
                char * var_names[64];
                Object * def_array[64];
                int32_t is_def;
                length = 0; // counter for var_names
                j = 0; // counter for def_array
                Object * assignments = GLOBAL_NULL;
                while (chunk != GLOBAL_NULL) {
                    is_def = true;
                    string = car(chunk)->data.String.v;
                    var_existed = false;
                    for (i = 0; i < length; i++) { // check in var_names
                        if (str_eq(var_names[i], string)) {
                            var_existed = true;
                            is_def = false;
                        }
                    }
                    if(!var_existed){ // 不存在 保存
                        var_names[length] = string;
                        length++;
                    }
                    def_array[j] = cons(is_def?DEF_STRING:SET_STRING,
                                        cons(car(chunk),
                                             cons(cadr(chunk), GLOBAL_NULL)));
                    chunk = cddr(chunk);
                    j++; //
                }
                // make assignments;
                for (i = j - 1; i >= 0; i--) {
                    assignments = cons(def_array[i],
                                       assignments);
                }
                temp =  cons(cons(LAMBDA_STRING,
                                  cons(GLOBAL_NULL,
                                       list_append(assignments,
                                                   cddr(l)))),
                             GLOBAL_NULL);
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
                return;
            }
            else if (str_eq(tag, "lambda")){
                Object * params = cadr(l); // get parameters
                int32_t variadic_place = -1; // variadic place
                int32_t counter = 0; // count of parameter num
                Variable_Table * vt_ = VT_copy(vt); // new variable table
                MacroTable * mt_ = MT_copy(mt); // new macro table
                VT_add_new_empty_frame(vt_); // we add a new frame
                MT_add_new_empty_frame(mt_); // we add a new frame
                
                //macros_.push([]); // add new frame
                //env_.push([]); // 必须加上这个， 要不然((lambda [] (defmacro square ([x] `[* ~x ~x])) (square 12))) macro 会有错
                while (true) {
                    if (params == GLOBAL_NULL) {
                        break;
                    }
                    if (car(params)->type != STRING) {
                        printf("ERROR: Invalid Function Parameter type\n");
                        return;
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
                Lambda_for_Compilation * function_ = malloc(sizeof(Lambda_for_Compilation));
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
                    }
                }
                free(vt_->frames[vt_->length - 1]->var_names);
                free(vt_->frames[vt_->length - 1]);
                free(vt_);
                
                // free macro table
                MacroTableFrame * top_frame = mt_->frames[mt_->length - 1];
                for (i = 0; i < top_frame->length; i++) {
                    Macro_free(top_frame->array[i]);
                }
                free(top_frame->array);
                free(top_frame);
                free(mt_);
                
                vt_ = NULL;
                mt_ = NULL;
                
                free(function_); // free lambda for compilation
                function_ = NULL;
                
                return;
            }
            /*
               (defmacro macro_name [var0 pattern0] [var1 pattern1] ...)
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
                        return;
                    }
                }
                // is not defined
                // resize if size is not enough
                if (frame->length == frame->size) {
                    frame->size*=2;
                    frame->array = realloc(frame->array, frame->size);
                }
                
                clauses->use_count+=1; // 必须+1
                frame->array[frame->length] = Macro_init(var_name->data.String.v, (clauses), VT_copy(vt));
                frame->length+=1;
                return;
            }
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
                        return;
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
                    jump_steps = -(insts->length - start_pc) + 1; // jump steps
                    Insts_push(insts, (0xFFFF0000 & jump_steps) >> 16);
                    Insts_push(insts, 0x0000FFFF & jump_steps);
                    return;
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
                    return;
                }
            }
        default:
            printf("ERROR: Invalid Data\n");
            return;
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
        if (cdr(l) == GLOBAL_NULL
            && car(l)->type == PAIR
            && parent_func_name != NULL
            && str_eq(car(car(l))->data.String.v, parent_func_name)) {
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
                     mt); // run vm
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
