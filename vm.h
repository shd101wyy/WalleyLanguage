﻿//
//  vm.h
//  walley
//
//  Created by WangYiyi on 4/28/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_vm_h
#define walley_vm_h
#include "garbage.h"

#define pop_param for(i = 0; i < param_num; i++){\
    temp = current_frame_pointer->array[current_frame_pointer->length - 1];\
    temp->use_count--; \
    Object_free(temp); \
    current_frame_pointer->length--;}
#define free_current_frame_pointer(current_frame_pointer) ((current_frame_pointer)->use_count)--; \
    EF_free((current_frame_pointer));

#define vm_error_jump Object_free(accumulator); \
accumulator = GLOBAL_NULL; \
goto VM_END; \

Object * Walley_Run_File_for_VM(char * file_name,
                                Instructions * insts,
                                Variable_Table * vt,
                                Environment * env,
                                MacroTable * mt);

/*
 Walley Language Virtual Machine
 */
Object *VM(/*uint16_t * instructions,*/
           Instructions * instructions_,
           uint64_t start_pc,
           uint64_t end_pc,
           Environment * env,
           Variable_Table * vt,
           MacroTable * mt,
           Module * module,
           Object * default_acc,
           Object * continuation_state){
    /*
     *  #################################################################################
     *  #################################################################################
     *             Some variable names that will be used in the future
     *  #################################################################################
     *  #################################################################################
     */
    uint16_t * instructions = instructions_->array;
    uint64_t pc;
    uint64_t i;
    uint16_t frame_index, value_index;
    int16_t inst;
    int16_t opcode;
    char param_num, variadic_place;
    uint64_t jump_steps;
    int32_t required_param_num, required_variadic_place;
    uint64_t string_length;
    char * created_string;
    int32_t s;
    char s1, s2;
    uint16_t offset;
    uint64_t hash_val;
    uint64_t integer__;
    Table_Pair * table_pairs;
    int64_t integer_;
    Environment_Frame * temp_frame;
    Environment * new_env;
    Object * (*func_ptr)(Object**, uint32_t); // function pointer
    Object * v;
    Object * temp; // temp use
    Object * temp2;
    
    /*
     *  #################################################################################
     *  #################################################################################
     *             Initialize Virtual Machine Variables
     *  #################################################################################
     *  #################################################################################
     */
    Environment * original_env = env; // save old env;
    
    Object * accumulator = (default_acc == NULL) ? GLOBAL_NULL : default_acc;
    
    Environment_Frame * global_frame = env->frames[0];
    
    Environment_Frame *BUILTIN_PRIMITIVE_PROCEDURE_STACK;
    
    Environment * continuation_env[MAX_STACK_SIZE];      // used to save env
    int16_t continuation_env_length = 0;                   // save length of that array
    
    uint64_t continuation_return_pc[MAX_STACK_SIZE]; // used to save return pc
    int16_t continuation_return_pc_length = 0;             // save length of that array
    
    Environment_Frame * frames_list[MAX_STACK_SIZE]; // save frame
    // frames_list[0] = NULL;
    int16_t frames_list_length = 0; //1;
    
    Object * functions_list[MAX_STACK_SIZE]; // save function
    int16_t functions_list_length = 0;
    

    /*
     *  Copy Continuation State if continuation_state is not null.
     */
    if (continuation_state != NULL) {
        
        Continuation_Saved_State * state = continuation_state->data.Continuation.state;
        
        // copy builtin_primitive_procedure_stack
        BUILTIN_PRIMITIVE_PROCEDURE_STACK = EF_copy(state->builtin_primitive_procedure_stack);
        BUILTIN_PRIMITIVE_PROCEDURE_STACK->use_count = 0;
        
        // copy continuation_env
        for (i = 0; i < state->continuation_env_length; i++) {
            continuation_env[i] = state->continuation_env[i];
        }
        continuation_env_length = state->continuation_env_length;
        
        // copy continuation_return_pc
        for (i = 0; i < state->continuation_return_pc_length; i++) {
            continuation_return_pc[i] = state->continuation_return_pc[i];
        }
        continuation_return_pc_length = state->continuation_return_pc_length;
        
        // copy frames_list
        for (i = 0; i < state->frames_list_length; i++) {
            temp_frame = EF_copy(state->frames_list[i]);
            frames_list[i] = temp_frame;
            if(temp_frame != NULL) temp_frame->use_count++;
        }
        frames_list_length = state->frames_list_length;
        
        // copy functions_list
        for (i = 0; i < state->functions_list_length; i++) {
            temp = state->functions_list[i];
            functions_list[i] = temp;
            temp->use_count++;
        }
        functions_list_length = state->functions_list_length;
    }
    else{
        BUILTIN_PRIMITIVE_PROCEDURE_STACK = EF_init_with_size(MAX_STACK_SIZE); // for builtin primitive procedure calculation
        BUILTIN_PRIMITIVE_PROCEDURE_STACK->use_count = 0; // cannot free it
    }
    
    
    
    // set current_frame_pointer
    Environment_Frame * current_frame_pointer = frames_list[frames_list_length - 1];
    /*
     * #################################################################################
     * #################################################################################
     *   run CONSTANT_TABLE_INSTRUCTIONS first to load constant
     * #################################################################################
     * #################################################################################
     */
    pc = CONSTANT_TABLE_INSTRUCTIONS_TRACK_INDEX;
    while (pc != CONSTANT_TABLE_INSTRUCTIONS->length) {
        inst = CONSTANT_TABLE_INSTRUCTIONS->array[pc];
        // opcode = (inst & 0xF000) >> 12;
        // 目前只支持string
        switch (inst) {
            case CONST_STRING: // push string to constant table
                string_length = (int64_t)CONSTANT_TABLE_INSTRUCTIONS->array[pc + 1]; // string length maximum 2 bytes
                created_string = (char*)malloc(sizeof(char) * (string_length + 1));
                pc = pc + 2;
                i = 0;
                while(1){
                    s = CONSTANT_TABLE_INSTRUCTIONS->array[pc];
                    s1 = (char)((0xFF00 & s) >> 8);
                    s2 = (char)(0x00FF & s);
                    if(s1 == 0x00) // reach end
                        break;
                    else{
                        created_string[i] = s1;
                        i++;
                    }
                    if(s2 == 0x00) // reach end
                        break;
                    else{
                        created_string[i] = s2;
                        i++;
                    }
                    pc = pc + 1;
                }
                created_string[i] = 0;
                
                // create string
                accumulator = Object_initString(created_string, string_length);
                accumulator->use_count = 1;
                if (Constant_Pool_Length == Constant_Pool_Size) {
                    Constant_Pool = (Object**)realloc(Constant_Pool, sizeof(Object*) * Constant_Pool_Size * 2);
                    Constant_Pool_Size *= 2;
                }
                // push to Constant_Pool
                Constant_Pool[Constant_Pool_Length] = accumulator;
                Constant_Pool_Length++;
                pc = pc + 1;
                
                free(created_string);
                continue;
            default:
                printf("ERROR: Invalid opcode for constant table");
                Object_free(accumulator);
                accumulator = GLOBAL_NULL;
                goto VM_END;
                break;
        }
    }
    CONSTANT_TABLE_INSTRUCTIONS_TRACK_INDEX = CONSTANT_TABLE_INSTRUCTIONS->length; // update track index for constant table instructions.
    /*
     * #################################################################################
     * #################################################################################
     */
    pc = start_pc;
    while(pc != end_pc){
        if (continuation_state != NULL && frames_list_length == 0) { // done continuation
            goto VM_END;
        }
        
    CONTINUE_VM:
        inst = instructions[pc];
        opcode = (inst & 0xF000) >> 12;
        //printf("%x\n", inst);
        switch(opcode){
            case SET:
                frame_index = 0x0FFF & inst;
                value_index = instructions[pc + 1];
                // printf("SET frame_index %d, value_index %d\n", frame_index, value_index);
                if ( value_index < env->frames[frame_index]->length) {
                    // free the existed value
                    v = env->frames[frame_index]->array[value_index];
                    v->use_count--; // decrement use_count
                    Object_free(v);
                }
                else{
                    
                    printf("VM ERROR: SET INDEX ERROR, plz report this error and send ur code to author through github...\n");
                    printf("value index %d, frame length %d\n", value_index, env->frames[frame_index]->length);
                    exit(0);
                }
                env->frames[frame_index]->array[value_index] = accumulator; // set value
                accumulator->use_count++;  // increase accumulator use_count
                pc = pc + 2;
                accumulator = GLOBAL_NULL;
                continue;
            case GET:
                frame_index = 0x0FFF & inst;
                value_index = instructions[pc + 1];
                
                // printf("GET frame_index %d, value_index %d\n", frame_index, value_index);
                
                Object_free(accumulator);
                
                accumulator = env->frames[frame_index]->array[value_index];
                // I think I need to change this later
                if (!accumulator) {
                    accumulator = GLOBAL_NULL;
                }
                pc = pc + 2;
                continue;
            case CONST:
                switch(inst){
                    case CONST_INTEGER:  // 64 bit num
                        // free accumulator is necessary
                        Object_free(accumulator);
                        
                        // 必须cast (uint64_t) 要不然有错
                        accumulator = Object_initInteger((int64_t)(((uint64_t)instructions[pc + 1] << 48) |
                                                                ((uint64_t)instructions[pc + 2] << 32) |
                                                                ((uint64_t)instructions[pc + 3] << 16) |
                                                                ((uint64_t)instructions[pc + 4])));
                        pc = pc + 5;
                        continue;
                    case CONST_FLOAT: // 64 bit double
                        // free accumulator is necessary
                        Object_free(accumulator);
                    
                        integer__ = (uint64_t)(((uint64_t)instructions[pc + 1] << 48) |
                                                                ((uint64_t)instructions[pc + 2] << 32) |
                                                                ((uint64_t)instructions[pc + 3] << 16) |
                                                                ((uint64_t)instructions[pc + 4]));
                       
                        accumulator = Object_initDouble(*((double*)(&integer__)));
                        pc = pc + 5;
                        continue;
                        
                    // case CONST_STRING: 这个会在CONSTANT_TABLE_INSTRUCTIONS里面运行
                    case CONST_LOAD:
                        // free accumulator if necessary
                        Object_free(accumulator);
                        
                        accumulator = Constant_Pool[instructions[pc + 1]]; // load constant
                        // printf("GET %s\n", accumulator->data.String.v);
                        pc = pc + 2;
                        continue;
                    case CONST_NULL: // null
                        // free accumulator if necessary
                        Object_free(accumulator);
                        // set null
                        accumulator = GLOBAL_NULL;
                        pc = pc + 1;
                        continue;
                    default:
                        printf("ERROR: Invalid constant\n");
                        Object_free(accumulator);
                        accumulator = GLOBAL_NULL;
                    goto VM_END;
                }
            case MAKELAMBDA: // make lambda
                param_num = (0x0FC0 & inst) >> 6;
                variadic_place = (0x0001 & inst) ? ((0x003E & inst) >> 1) : -1;
                start_pc = pc + 3;
                jump_steps = instructions[pc + 1];
                
                // free accumulator is necessary
                Object_free(accumulator);
                
                accumulator = Object_initUserDefinedLambda(param_num, variadic_place, start_pc, copyEnvironment(env), (uint8_t)instructions[pc + 2]);
                pc = pc + jump_steps;
                continue;
            case RETURN:
            // return_label:;
                // printf("RETURN \n");
                accumulator->use_count++; // because accumulator may exist on frame
                // free top frame
                temp_frame = env->frames[env->length - 1]; // get top frame
                temp_frame->use_count--;
                EF_free(temp_frame);
                free(env->frames);
                free(env);
                accumulator->use_count--; // restore accumulator use count
                
                
                pc = continuation_return_pc[continuation_return_pc_length - 1]; // get old pc
                continuation_return_pc_length-=1;
                
                env = continuation_env[continuation_env_length - 1]; // get old env
                continuation_env_length-=1;
                continue;
            case NEWFRAME: // create new frame
                // printf("NEWFRAME \n");
                switch (accumulator->type){
                        /*
                         *
                         * Only USER_DEFINED_LAMBDA will need to create new frame
                         *
                         */
                    case USER_DEFINED_LAMBDA: // user defined function
                        // create new frame with length 64
                        current_frame_pointer = EF_init_with_size(accumulator->data.User_Defined_Lambda.frame_size);
                        
                        // save to frames_list
                        frames_list[frames_list_length] = current_frame_pointer;
                        frames_list_length+=1;
                        current_frame_pointer->use_count++; // current frame pointer is used

                        // save to function list
                        functions_list[functions_list_length] = accumulator;
                        functions_list_length++;
                        accumulator->use_count++;
                        pc++;
                        continue;
                        /*
                         *
                         * Those data types will only need to use BUILTIN_PRIMITIVE_PROCEDURE_STACK.
                         *
                         */
                    case STRING: case BUILTIN_LAMBDA: case VECTOR: case TABLE: case  INTEGER: case OBJECT: case CONTINUATION: // builtin lambda or vector or table
                        current_frame_pointer = BUILTIN_PRIMITIVE_PROCEDURE_STACK; // get top frame
                        // current_frame_pointer->use_count++; // NO NEED TO CHANGE use_count for BUILTIN_PRIMITIVE_PROCEDURE_STACK
                        
                        // save to frame list
                        frames_list[frames_list_length] = NULL; // set to NULL, NULL means BUILTIN_PRIMITIVE_PROCEDURE_STACK.  // current_frame_pointer;
                        frames_list_length++;
                        
                        // save to function list
                        functions_list[functions_list_length] = accumulator;
                        functions_list_length++;
                        accumulator->use_count++;
                        
                        pc++;
                        continue;
                    default:
                        printf("ERROR: NEWFRAME error");
                        printf("       %s\n", to_string(accumulator));
                        Object_free(accumulator);
                        accumulator = GLOBAL_NULL;
                    goto VM_END;
                }
            case PUSH_ARG: // push arguments
                // printf("PUSH_ARG \n");
                accumulator->use_count++; // increase use count
                if (current_frame_pointer == NULL) {
                    BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length] = accumulator;
                    BUILTIN_PRIMITIVE_PROCEDURE_STACK->length++;
                }
                else{
                    current_frame_pointer->array[current_frame_pointer->length] = accumulator; // push to env frame
                    current_frame_pointer->length++;
                }
                pc++;
                continue;
                
            case CALL:
                // printf("CALL \n");
                param_num = (0x0FFF & inst);
                v = functions_list[functions_list_length - 1]; // get function
                functions_list[functions_list_length - 1] = NULL; // clear
                functions_list_length--;  // pop that function from list
                v->use_count--; // decrement use count
                
                /*
                 * In usual, accumulator is the calling function, or its last argument.
                 */
                accumulator = GLOBAL_NULL; // 必须设为 GLOBAL_NULL
                
                switch (v->type){
                    case STRING: // only support access
                        pc = pc + 1;
                        temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1];
                        integer_ = temp->data.Integer.v; // get index
                        
                        char b_[2];
                        b_[0] = v->data.String.v[integer_];
                        accumulator = Object_initString(b_, 1);
                        // accumulator = v->data.String.v[integer_]; // get value
                        
                        // Assume here we only have one parameter. so only need to pop once
                        temp->use_count--; // pop parameters.
                        Object_free(temp);
                        BUILTIN_PRIMITIVE_PROCEDURE_STACK->length--; // decrease length.
                        
                        frames_list_length--; // pop frame list
                        current_frame_pointer = frames_list[frames_list_length - 1];

                        // free lambda
                        Object_free(v);
                        continue;

                    case BUILTIN_LAMBDA: // builtin lambda
                        func_ptr = v->data.Builtin_Lambda.func_ptr;
                        accumulator = (*func_ptr)(&(BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - param_num]), param_num); // call function
                    eval_builtin_lambda:
                        accumulator->use_count++; //必须在pop parameters之前运行这个 eg (car '((x))) 得到了 (x)， 但是如果 accumulator->use_count不加加的话 (x)会被free掉，
                        // 在 pop 完 parameters之后在 decrease accumulator->use_count
                        // pop parameters
                        for(i = 0; i < param_num; i++){
                            temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1];

                            temp->use_count--; // －1 因为在push的时候加1了
                            
                            Object_free(temp); // free object
                            
                            // BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1] = NULL; // clear
                            BUILTIN_PRIMITIVE_PROCEDURE_STACK->length--; // decrease length
                        }
                        accumulator->use_count--;
                        
                        pc = pc + 1;
                        
                        frames_list_length--; // pop frame list
                        current_frame_pointer = frames_list[frames_list_length - 1];
                    
                        // free lambda
                        Object_free(v);
                        continue;
                    case VECTOR: // vector
                        pc = pc + 1;
                        switch(param_num){
                            case 1: // vector get
                                temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1];
                                integer_ = temp->data.Integer.v; // get index
                                accumulator = v->data.Vector.v[integer_]; // get value
                                
                                temp->use_count--; // pop parameters
                                Object_free(temp);
                                BUILTIN_PRIMITIVE_PROCEDURE_STACK->length--; // decrease length
                                
                                frames_list_length--; // pop frame list
                                current_frame_pointer = frames_list[frames_list_length - 1];
                                
                                // free lambda
                                Object_free(v);
                                
                                continue;
                            case 2: // vector set
                                temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 2]; // index
                                temp2 = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1]; // value
                                integer_ = temp->data.Integer.v;
                                
                                // decrease use_count of old_value
                                v->data.Vector.v[integer_]->use_count--;
                                Object_free(v->data.Vector.v[integer_]);
                                
                                // set to vector
                                v->data.Vector.v[integer_] = temp2;
                                temp2->use_count++; // in use
                                
                                // pop parameters
                                for(i = 0; i < param_num; i++){
                                    temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1];
                                    temp->use_count--; // －1 因为在push的时候加1了
                                    Object_free(temp);
                            
                                    BUILTIN_PRIMITIVE_PROCEDURE_STACK->length--; // decrease length
                                }
                                
                                frames_list_length--; // pop frame list
                                current_frame_pointer = frames_list[frames_list_length - 1];
                                
                                // free lambda
                                Object_free(v);

                                continue;
                            default: // wrong parameters
                                printf("ERROR: Invalid vector operation\n");
                                Object_free(accumulator);
                                accumulator = GLOBAL_NULL;
                            goto VM_END;
                        }
                    
                    case TABLE: // table
                        pc = pc + 1;
                        switch(param_num){
                                /*
                                 * eg (def x {:type 'Object})
                                 * x:type => 
                                 * 1000 7a 5000 2500 10c 6000 7001
                                 * 变为
                                 * 1000 7a   0xE        xxxx          xxxx xxxx        xxxx
                                 *           get_table  symbol_index   table_offset    not used
                                 */
                            case 1: // table get
                                temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1];
                               
                                // get value from table
                                /*
                                accumulator = Table_getval(v, temp);
                                */
                                hash_val = hash(temp, v->data.Table.size); // get hash value
                                table_pairs = v->data.Table.vec[hash_val];
                                while (table_pairs!=NULL) {
                                    if (table_pairs->key == temp ||
                                        (table_pairs->key->type == STRING
                                         && temp->type == STRING
                                         && strcmp(temp->data.String.v, table_pairs->key->data.String.v) == 0)) {
                                        accumulator = table_pairs->value;
                                        
                                        /*
                                         * change instructions.
                                         */
                                        if (instructions[pc - 5] == NEWFRAME << 12 && /*instructions[pc - 4] == 0x5000 &&*/ instructions[pc - 4] == 0x2500) {
                                            instructions[pc - 5] = TABLE_GET << 12;
                                            instructions[pc - 4] = instructions[pc - 3]; // symbol offset
                                            instructions[pc - 3] = (hash_val & 0xFFFF0000) >> 16;
                                            instructions[pc - 2] = (hash_val & 0xFFFF);
                                            instructions[pc - 1] = 0; // not used
                                        }
                                        
                                        goto TABLE_FINISH_FINDING_VALUE;
                                    }
                                    table_pairs = table_pairs->next;
                                }
                                accumulator = GLOBAL_NULL;
                                
                            TABLE_FINISH_FINDING_VALUE:
                                temp->use_count--; // pop parameters
                                Object_free(temp);
                                BUILTIN_PRIMITIVE_PROCEDURE_STACK->length--; // decrease length
                                
                                frames_list_length--; // pop frame list
                                current_frame_pointer = frames_list[frames_list_length - 1];
                                
                                // free lambda
                                Object_free(v);
                                continue;

                            case 2: // table set
                                temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 2]; // key
                                temp2 = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1]; // value
                                
                                Table_setval(v, temp, temp2);
                                
                                // 下面这个不用运行
                                // 因为在 Table_setval的时候会自动增加
                                // temp2->use_count++; // in use
                                
                                // pop parameters
                                for(i = 0; i < param_num; i++){
                                    temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1];
                                    temp->use_count--; // －1 因为在push的时候加1了
                                    Object_free(temp);
                                    
                                    BUILTIN_PRIMITIVE_PROCEDURE_STACK->length--; // decrease length
                                }
                                
                                frames_list_length--; // pop frame list
                                current_frame_pointer = frames_list[frames_list_length - 1];
                                
                                // free lambda
                                Object_free(v);
                                
                                continue;
                            default: // wrong parameters
                                printf("ERROR: Invalid table operation\n");
                                Object_free(accumulator);
                                accumulator = GLOBAL_NULL;
                            goto VM_END;
                        }
                        /*
                         *
                         * The current_frame_pointer for USER_DEFINED_LAMBDA is not BUILTIN_PRIMITIVE_PROCEDURE_STACK
                         *
                         */
                    case USER_DEFINED_LAMBDA: // user defined function
                    eval_user_defined_lambda:
                        //printf("USER_DEFINED_LAMBDA\n");
                        required_param_num = v->data.User_Defined_Lambda.param_num;
                        required_variadic_place = v->data.User_Defined_Lambda.variadic_place;
                        start_pc = v->data.User_Defined_Lambda.start_pc;
                        
                        // create new environment
                        new_env = copyEnvironmentAndPushFrame(v->data.User_Defined_Lambda.env, current_frame_pointer);
                        
                        if(required_variadic_place == -1 && param_num - 1 > required_param_num){
                            printf("ERROR: Too many parameters provided\n");
                            Object_free(accumulator);
                            accumulator = GLOBAL_NULL;
                            goto VM_END;
                        }
                        if(required_variadic_place != -1){
                            v = GLOBAL_NULL;
                            int32_t m; // I used to use i, but it somehow will cause problem... showed 64 bit integer 18...
                            for(m = current_frame_pointer->length - 1; m >= required_variadic_place; m=m-1){
                                current_frame_pointer->array[m]->use_count--; // 因为 cons的时候会再增加
                                v = cons(current_frame_pointer->array[m], v);
                                //current_frame_pointer->array[i] = NULL; // clear
                            }
                            current_frame_pointer->array[required_variadic_place] = v;
                            v->use_count++;
                            current_frame_pointer->length = required_variadic_place + 1; // update length
                        }
                        
                        // set null
                        if((current_frame_pointer->length) < required_param_num){
                            // param_num here is wrong, cuz if the fn is object fn, then the param_num
                            // should be param_num + 1.
                            // therefore I changed it to current_frame_pointer->length.
                            for(i = /*param_num*/current_frame_pointer->length; i < required_param_num; i++){
                                current_frame_pointer->array[i] = GLOBAL_NULL;
                                GLOBAL_NULL->use_count++; // 用吗？
                            }
                            current_frame_pointer->length += (required_param_num - current_frame_pointer->length/*param_num*/); // update length
                        }
                        
                        // save return pc
                        continuation_return_pc[continuation_return_pc_length] = pc+1;
                        continuation_return_pc_length++;
                        // save old env
                        continuation_env[continuation_env_length] = env;
                        continuation_env_length++;
                        
                        // reset pointers
                        env = new_env;
                        pc = start_pc;
                        
                        /* no need to free current_frame_pointer, as it will be freed after calling RETURN */
                        // free current_frame_pointer
                        // free_current_frame_pointer(current_frame_pointer);
            
                        /* 前面的 copyEnvironmentAndPushFrame 里面的 current_frame_pointer 的 use_count会++, 所以这里得 -- */
                        current_frame_pointer->use_count--;
                        
                        frames_list_length--; // pop frame list
                        current_frame_pointer = frames_list[frames_list_length - 1];
                        
                        // free lambda
                        //if(accumulator != v) //这里是处理 ((lambda [] 'x))的 free bug
                        Object_free(v);
                        continue;
                        
                    case INTEGER:
                        switch (v->data.Integer.v) {
                            case 1: // eval
                                pc = pc + 1;
                                temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - param_num]; // get first parameter
                                
                                if (vt == NULL) {
                                    printf("ERROR: eval function is only run time supported");
                                    vm_error_jump
                                }
                                
                                // get value to compile
                                temp = temp->type == STRING ? parser(lexer(temp->data.String.v))
                                : cons(temp, GLOBAL_NULL);
                                
                                // reset start_pc
                                instructions_->start_pc = instructions_->length;
                                // run
                                accumulator = compiler_begin(instructions_,
                                                             temp,
                                                             vt,
                                                             NULL,
                                                             NULL,
                                                             1,
                                                             original_env,
                                                             mt,
                                                             module); // eval 的默认 namespace 名称是空
                                // Object_free(temp); // compiler_begin 会给 free 掉
                                
                                // restore start-pc
                                instructions_->start_pc = start_pc;
                                
                                accumulator->use_count++; //必须在pop
                                // pop parameters
                                for(i = 0; i < param_num; i++){
                                    temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1];
                                    
                                    temp->use_count--; // －1 因为在push的时候加1了
                                    
                                    Object_free(temp); // free object
                                    
                                    BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1] = NULL; // clear
                                    BUILTIN_PRIMITIVE_PROCEDURE_STACK->length--; // decrease length
                                }
                                accumulator->use_count--;
                                
                                frames_list_length--; // pop frame list
                                current_frame_pointer = frames_list[frames_list_length - 1];
                                
                                // free lambda
                                // this cannot be freed because it is a builtin-function
                                // Object_free(v);
                                continue;
                            case 2: // apply
                                // pc = pc + 1; //不同 +1 因为后面goto后会改变pc
                                v = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - param_num]; // get func
                                temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - param_num + 1];
                                    // get params
                                switch (v->type) {
                                    case BUILTIN_LAMBDA: // builtin lambda
                                                         // push parameters to stack
                                        param_num = 0; // include apply and temp
                                        while (temp != GLOBAL_NULL) {
                                            BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length] = car(temp);
                                            car(temp)->use_count++;
                                            BUILTIN_PRIMITIVE_PROCEDURE_STACK->length++;
                                            temp = cdr(temp);
                                            param_num++;
                                        }
                                        accumulator = v->data.Builtin_Lambda.func_ptr(&(BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - param_num]),
                                                    param_num);
                                        param_num+=2; // include apply and temp
                                        goto eval_builtin_lambda;
                                        
                                    case USER_DEFINED_LAMBDA: // user defined lambda
                                        accumulator = GLOBAL_NULL; // clear accumulator, otherwise it will cause error. because it might be freed more than one times
                                        temp_frame = EF_init_with_size(64); // create temp frame for user defined lambda
                                        
                                        while (temp != GLOBAL_NULL) {
                                            temp_frame->array[temp_frame->length] = car(temp);
                                            car(temp)->use_count++;
                                            temp = cdr(temp);
                                            temp_frame->length++;
                                        }
                                        temp_frame->use_count++;
                                        
                                        // pop parameters
                                        for(i = 0; i < param_num; i++){
                                            temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1];
                                            temp->use_count--; // －1 因为在push的时候加1了
                                            Object_free(temp);
                                            
                                            BUILTIN_PRIMITIVE_PROCEDURE_STACK->length--; // decrease length
                                        }
                                        
                                        current_frame_pointer = temp_frame;
                                        goto eval_user_defined_lambda;
                                    default:
                                        printf("ERROR1: Invalid lambda\n");
                                        vm_error_jump
                                }
                            case 3:
                                // v is the fn in call/cc
                                // eg (call/cc my-func)  v = my-func
                                if (param_num != 1) {
                                    printf("ERROR: call/cc wrong number argument\n");
                                    vm_error_jump
                                }
                                
                                v = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - param_num]; // get call/cc fn argument
                                if(v->type != USER_DEFINED_LAMBDA){
                                    printf("ERROR: call/cc invalid argument\n");
                                    vm_error_jump
                                }
                                
                                accumulator = GLOBAL_NULL;
                                temp_frame = EF_init_with_size(64);
                                
                                // v is on Builtin_Stack,
                                // and current_frame_pointer => Builtin_Stack
                                // so we cannot free v
                                /*
                                v->use_count++;
                                // pop parameters
                                for(i = 0; i < param_num; i++){
                                    temp = current_frame_pointer->array[current_frame_pointer->length - 1];
                                    temp->use_count--; // －1 因为在push的时候加1了
                                    Object_free(temp);
                                    current_frame_pointer->length--; // decrease length
                                }
                                v->use_count--;
                                 */
                                current_frame_pointer->length -= param_num;
                                
                                // ############################################
                                // create Continuation
                                Object * continuation =
                                    Object_initContinuation(pc + 1,
                                                            copyEnvironment(env)); // TODO : init continuation
                                
                                // copy builtin_primitive_procedure_stack
                                Continuation_Saved_State * state = continuation->data.Continuation.state;
                                state->builtin_primitive_procedure_stack = EF_copy(BUILTIN_PRIMITIVE_PROCEDURE_STACK);
                                state->builtin_primitive_procedure_stack->use_count = 0;
                                
                                // copy continuation_env
                                state->continuation_env = malloc(sizeof(Environment_Frame*) * continuation_env_length);
                                for (i = 0; i < continuation_env_length; i++) {
                                    state->continuation_env[i] = continuation_env[i];
                                }
                                state->continuation_env_length = continuation_env_length;
                                
                                // copy continuation_return_pc
                                state->continuation_return_pc = malloc(sizeof(uint64_t)*continuation_return_pc_length);
                                for (i = 0; i < continuation_return_pc_length; i++) {
                                    state->continuation_return_pc[i] = continuation_return_pc[i];
                                }
                                state->continuation_return_pc_length = continuation_return_pc_length;
                                
                                // copy frames_list
                                state->frames_list = malloc(sizeof(Environment_Frame*) * frames_list_length);
                                for (i = 0; i < frames_list_length - 1; i++) { // the 1st one is NULL, has no use_count
                                    state->frames_list[i] = EF_copy( frames_list[i] );
                                    if(frames_list[i] != NULL) frames_list[i]->use_count++;
                                }
                                state->frames_list_length = frames_list_length - 1;
                                
                                // copy functions_list
                                state->functions_list = malloc(sizeof(Object*) * functions_list_length);
                                for (i = 0; i < functions_list_length; i++) {
                                    state->functions_list[i] = functions_list[i];
                                    functions_list[i]->use_count++;
                                }
                                state->functions_list_length = functions_list_length;
                                
                                // increase use_count for continuation cuz it will be append to current frame.
                                continuation->use_count++;
                                // ######################################
                                
                                // add continuation object to frame
                                temp_frame->array[0] = continuation;
                                temp_frame->length = 1;
                                
                                // Because for call/cc
                                // current_frame_pointer is Builtin_Stack
                                // no need to free.
                                current_frame_pointer = temp_frame;
                                
                                current_frame_pointer->use_count++; // assume current_frame_pointer now is in frame_list.
                                
                                goto eval_user_defined_lambda;
            
                            default:
                                printf("ERROR2: Invalid Lambda\n");
                                // TODO: Object_free(v)
                                Object_free(accumulator);
                                accumulator = GLOBAL_NULL;
                                goto VM_END;
                        }
                    case CONTINUATION:
                        //return BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - param_num];
                        // THIS BLOCK OF CODE HAS MEMORY LEAK
                        // eg
                        // (def return 0) (+ 1 (call/cc (fn (i) (set! return i) (+ 2 (i 3)))))
                        if (pc < v->data.Continuation.pc) { // inside continuation function. // break current function
                            //printf("call/cc in\n");
                            //printf("@@ frames_list_length %d\n", frames_list_length - 1);
                            accumulator = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - param_num];
                            
                            
                            // get continuation saved state
                            Continuation_Saved_State * state = v->data.Continuation.state;
                            
                            accumulator->use_count++;
                            // restore BUILTIN_PRIMITIVE_PROCEDURE_STACK
                            while (BUILTIN_PRIMITIVE_PROCEDURE_STACK->length != state->builtin_primitive_procedure_stack->length) {
                                temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1];
                                temp->use_count--;
                                Object_free(temp);
                                BUILTIN_PRIMITIVE_PROCEDURE_STACK->length--;
                            }
                            // restore to correct continuation_env
                            while (continuation_env_length != state->continuation_env_length) {
                                env = continuation_env[continuation_env_length - 1];
                                if (continuation_env_length != 1) { // the 0th one is original env, shouldn't be freed
                                    temp_frame = env->frames[env->length - 1]; // free top frame
                                    temp_frame->use_count--;
                                    EF_free(temp_frame);
                                    free(env->frames);
                                    free(env);
                                }
                                continuation_env_length--;
                            }
                            env = state->continuation_env_length == 0 ? original_env : continuation_env[state->continuation_env_length - 1];
                            
                            // restore to correct continuation_return_pc
                            continuation_return_pc_length = state->continuation_return_pc_length;
                            
                            // restore to correct frames_list
                            while (frames_list_length != state->frames_list_length) {
                                current_frame_pointer = frames_list[frames_list_length - 1];
                                if (current_frame_pointer != NULL) {
                                    current_frame_pointer->use_count--;
                                    EF_free(current_frame_pointer);
                                }
                                frames_list_length--;
                            }
                            current_frame_pointer = state->frames_list_length == 0 ? NULL : frames_list[state->frames_list_length - 1];
                            
                            // restore to correct functions list
                            while (functions_list_length != state->functions_list_length) {
                                temp = functions_list[functions_list_length - 1];
                                temp->use_count--;
                                Object_free(temp);
                                functions_list_length--;
                            }
                            
                            pc = v->data.Continuation.pc;
                        }
                        else{ // outside continuation function
                            //printf("call/cc out\n");
                            pc += 1;
                            // run
                            accumulator = VM(instructions_,
                                             v->data.Continuation.pc,
                                             instructions_->length,
                                             v->data.Continuation.env,
                                             vt,
                                             mt,
                                             module,
                                             BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - param_num], // the first argument
                                              v);
                            /*
                            printf("frames_list_length %d %d\n", frames_list_length, v->data.Continuation.state->frames_list_length);
                            printf("functions_list_length %d %d\n", functions_list_length, v->data.Continuation.state->functions_list_length);
                            printf("continuation_env %d %d\n", continuation_env_length, v->data.Continuation.state->continuation_env_length);
                            printf("continuation_pc %d %d\n", continuation_return_pc_length, v->data.Continuation.state->continuation_return_pc_length);
                            printf("cuurent env frames length %d\n", env->length);
                             */
                            return accumulator;
                        }
                        
                        //printf("@@ acc %s\n", to_string(accumulator));
                        //accumulator->use_count--;
                        // free continuation if necessary
                        //Object_free(v);
                        continue;
                    case OBJECT:
                        // printf("UNFINISHED IMPLEMENTATION Object\n");
                        pc++;
                        switch(param_num){
                            /*
                             * 这种情况就是第一次查找property
                             * 例如 Object:type
                             * 1000 44   5000           2500 10c       6000     7001
                             *  get_obj  make_frame     get :type    push     call
                             *
                             * 改变 instructions 为
                             * 如果是从protos里面拿到的properties:
                             * 1000 44       bXXX          xxxx xxxx      xxxx xxxx
                             * get_obj  get_property       object_id       action address
                             * 如果是从自己拿到的properties:
                             * 1000 44       c xxxx           xxxx xxxx             xxxx
                             * get_obj        get property     not used       action offset
                             */
                            case 1: // Object get
                                temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1]; // get symbol
                                temp2 = v; // save v;
                                
                            // get value from object
                            OBJECT_FIND_PROPERTY:
                                hash_val = hash(temp, v->data.Table.size); // get hash value
                                table_pairs = v->data.Table.vec[hash_val];
                                while (table_pairs!=NULL) {
                                    if (table_pairs->key == temp ||
                                        (table_pairs->key->type == STRING
                                         && temp->type == STRING
                                         && strcmp(temp->data.String.v, table_pairs->key->data.String.v) == 0)) {
                                        accumulator = table_pairs->value;
                                        
                                        /*
                                         * change instructions.
                                         */
                                        /*
                                        if (instructions[pc - 5] == NEWFRAME << 12 && instructions[pc - 4] == 0x5000) {
                                            instructions[pc - 5] = TABLE_GET << 12;
                                            instructions[pc - 4] = instructions[pc - 3]; // symbol offset
                                            instructions[pc - 3] = (hash_val & 0xFFFF0000) >> 16;
                                            instructions[pc - 2] = (hash_val & 0xFFFF);
                                            instructions[pc - 1] = 0; // not used
                                        }
                                         */
                                        goto OBJECT_FINISH_FINDING_VALUE;
                                    }
                                    table_pairs = table_pairs->next;
                                }
                                if (v->data.Table.proto == GLOBAL_NULL) {
                                    // didn't find
                                    accumulator = GLOBAL_NULL;
                                }
                                else{
                                    v = v->data.Table.proto;
                                    goto OBJECT_FIND_PROPERTY;
                                }
                                
                            OBJECT_FINISH_FINDING_VALUE:
                                temp->use_count--; // pop parameters
                                Object_free(temp);
                                BUILTIN_PRIMITIVE_PROCEDURE_STACK->length--; // decrease length
                                
                                frames_list_length--; // pop frame list
                                current_frame_pointer = frames_list[frames_list_length - 1];
                                
                                /*
                                 * If call object method later
                                 * push that object to function as well
                                 */
                                if (instructions[pc] == NEWFRAME << 12 &&
                                    (accumulator->type == USER_DEFINED_LAMBDA)) {
                                         // create new frame with length 64
                                         if (accumulator->type == USER_DEFINED_LAMBDA) {
                                             current_frame_pointer = EF_init_with_size(accumulator->data.User_Defined_Lambda.frame_size);
                                         }
                                         else{
                                             current_frame_pointer = BUILTIN_PRIMITIVE_PROCEDURE_STACK;
                                         }
                                         
                                         
                                         // save to frames_list
                                         frames_list[frames_list_length] = current_frame_pointer;
                                         frames_list_length++;
                                         current_frame_pointer->use_count++; // current frame pointer is used
                                         
                                         // save to function list
                                         functions_list[functions_list_length] = accumulator;
                                         functions_list_length++;
                                         accumulator->use_count++;
                                         
                                         // push self
                                         current_frame_pointer->array[current_frame_pointer->length] = temp2; // push to env frame
                                         current_frame_pointer->length++;
                                         temp2->use_count++;
                                    
                                         pc++;
                                         continue;
                                     }
                                else{
                                    // free lambda
                                    Object_free(temp2); // temp2 is the original v
                                }
                                
                                continue;
                            case 2: // Object set
                                    // same as table set
                                temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 2]; // key
                                temp2 = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1]; // value
                                
                                Table_setval(v, temp, temp2);
                                
                                // 下面这个不用运行
                                // 因为在 Table_setval的时候会自动增加
                                // temp2->use_count++; // in use
                                
                                // pop parameters
                                for(i = 0; i < param_num; i++){
                                    temp = BUILTIN_PRIMITIVE_PROCEDURE_STACK->array[BUILTIN_PRIMITIVE_PROCEDURE_STACK->length - 1];
                                    temp->use_count--; // －1 因为在push的时候加1了
                                    Object_free(temp);
                                    
                                    BUILTIN_PRIMITIVE_PROCEDURE_STACK->length--; // decrease length
                                }
                                
                                frames_list_length--; // pop frame list
                                current_frame_pointer = frames_list[frames_list_length - 1];
                                
                                // free lambda
                                Object_free(v);
                                continue;

                            default: // wrong parameters
                                printf("ERROR: Invalid vector operation\n");
                                Object_free(accumulator);
                                accumulator = GLOBAL_NULL;
                                goto VM_END;
                        }
                    default:
                        printf("ERROR3: Invalid Lambda\n");
                        Object_free(accumulator);
                        accumulator = GLOBAL_NULL;
                        goto VM_END;
                }
            case JMP:
                //printf("JUMP STEPS %d\n", (signed int)((instructions[pc + 1] << 16) | instructions[pc + 2]));
                pc = pc + (int32_t)((instructions[pc + 1] << 16) | instructions[pc + 2]);
                continue;
                
            case TEST:
                if (accumulator->type == NULL_){
                    pc = pc + (uint16_t)instructions[pc + 1];
                    continue;
                }
                pc = pc + 2;
                continue;
            // 不知道到底用不用是有这个opcode
            // def
            // 这个SET_TOP和TAIL_CALL_PUSH几乎一样
            case SET_TOP: // set to top frame according to index
                // set value and increase length
                temp = env->frames[env->length-1]->array[instructions[pc+1]]; // get original
                accumulator->use_count++;
                if (temp) { // free old if necessary
                    temp->use_count--;
                    Object_free(temp); // free old
                }
                else{ // new one. 原来不存在过，所以 length 要 ++。
                    env->frames[env->length - 1]->length++; // increase length;
                }
                env->frames[env->length-1]->array[instructions[pc+1]] = accumulator;
                pc+=2;
                accumulator = GLOBAL_NULL;
                continue;
            // tail call push parameters
            // opcode index
            case TAIL_CALL_PUSH:
                offset = instructions[pc] & 0x0FFF;
                temp = env->frames[env->length-1]->array[offset]; // get original
                accumulator->use_count++;
                if (temp) {   // free old if necessary
                    temp->use_count--;
                    Object_free(temp); // free old
                }
                else{ // new one. 原来不存在过，所以 length 要 ++。
                    env->frames[env->length - 1]->length++; // increase length;
                }
                env->frames[env->length-1]->array[offset] = accumulator; // set new
                pc++;
                accumulator = GLOBAL_NULL; // 必须set为global null， 要不然会出错， 因为GET的时候会free掉accumulator, 而此时的accumulator的use count正好是0
                continue;
            case OBJECT_GET_SELF:
                
                continue;
            case OBJECT_GET_PARENT:

                continue;
            case TABLE_GET:
                // printf("TABLE_GET \n");
                offset = instructions[pc + 1]; // symbol offset
                temp = Constant_Pool[offset]; // get symbol
                hash_val = ((uint64_t)instructions[pc + 2] << 16) |
                           ((uint64_t)instructions[pc + 3]);
                if (hash_val > accumulator->data.Table.size) { // invalid hash_val
                    Object_free(accumulator);
                    accumulator = GLOBAL_NULL;
                    pc += 5;
                    goto CONTINUE_VM;
                }
                i = -1;
            TABLE_GET_BEGIN_TO_FIND_VALUE:
                v = accumulator; // get table.
                table_pairs = v->data.Table.vec[hash_val];
                while (table_pairs != NULL) {
                    if (table_pairs->key == temp || strcmp(temp->data.String.v, table_pairs->key->data.String.v) == 0) { // find
                        accumulator = table_pairs->value;
                        accumulator->use_count++;
                        Object_free(v);
                        accumulator->use_count--;
                        pc = pc + 5;
                        goto CONTINUE_VM;
                    }
                    table_pairs = table_pairs->next;
                }
                if (i == 0) { // 第一次没找到
                    // didn't find
                    hash_val = hash(temp, v->data.Table.size); // rehash
                    i++;
                    goto TABLE_GET_BEGIN_TO_FIND_VALUE;
                }
                else{ // 第二次没找到
                    accumulator = GLOBAL_NULL;
                    Object_free(v);
                    pc = pc + 5;
                    continue;
                }
                
            case GLOBAL_PUSH: // push global variable
                accumulator->use_count++;
                global_frame->array[instructions[pc + 1]] = accumulator;
                global_frame->length++;
                /*
                 TODO :
                    这个以后是为了 compile 得到 .wac file之后
                    还可以eval, 或者 load .wac file
                 
                if (GLOBAL_PUSH_SAVE_TO_VT) {
                    printf("need to save\n");
                    Module_addOffset(module, instructions[pc + 1]);
                    global_vtf->var_names[instructions[pc + 1]] = Constant_Pool[instructions[pc + 2]]->data.String.v;
                    printf("save %s\n", Constant_Pool[instructions[pc + 2]]->data.String.v);
                }
                else{
                    printf("doesn't need to save\n");
                }*/
                pc += 3;
                continue;
            default:
                printf("ERROR: Invalid opcode %d\n", opcode);
                vm_error_jump
        }
    }
    
VM_END:
    // free BUILTIN_PRIMITIVE_PROCEDURE_STACK
    accumulator->use_count+=1;
    
    EF_free(BUILTIN_PRIMITIVE_PROCEDURE_STACK);
    
    accumulator->use_count-=1;

    return accumulator;
}

























#endif
