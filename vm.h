//
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
Object *VM(uint16_t * instructions,
           uint64_t start_pc,
           uint64_t end_pc,
           Environment * env,
           Variable_Table * vt,
           MacroTable * mt){
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
    
    int64_t integer_;
    // double double_;
    Environment * original_env = env; // save old env;
    Object * accumulator = GLOBAL_NULL;
    Environment_Frame * current_frame_pointer = NULL;
    Environment_Frame * temp_frame;
    Environment * new_env;
    Object * (*func_ptr)(Object**, uint32_t, uint32_t); // function pointer
    Object * v;
    Object * temp; // temp use
    Object * temp2;
    
    Environment_Frame *BUILTIN_PRIMITIVE_PROCEDURE_STACK = EF_init_with_size(MAX_STACK_SIZE); // for builtin primitive procedure calculation
    BUILTIN_PRIMITIVE_PROCEDURE_STACK->use_count = 1; // cannot free it
    
    Environment * continuation_env[MAX_STACK_SIZE];      // used to save env
    int16_t continuation_env_length = 0;                   // save length of that array
    uint64_t continuation_return_pc[MAX_STACK_SIZE]; // used to save return pc
    int16_t continuation_return_pc_length = 0;             // save length of that array
    Environment_Frame * frames_list[MAX_STACK_SIZE]; // save frame
    frames_list[0] = NULL;
    int16_t frames_list_length = 1;
    Object * functions_list[MAX_STACK_SIZE]; // save function
    int16_t functions_list_length = 0;
    
    pc = CONSTANT_TABLE_INSTRUCTIONS_TRACK_INDEX;
    // run CONSTANT_TABLE_INSTRUCTIONS first to load constant
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
                // push to Constant_Pool
                // printf("PUSH %s %d\n", accumulator->data.String.v, Constant_Pool_Length);
                Constant_Pool[Constant_Pool_Length] = accumulator;
                Constant_Pool_Length++;
                pc = pc + 1;
                
                free(created_string);
                continue;
                break;
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
    for (i = start_pc; i < end_pc; i++) {
        printf("%x ", instructions[i]);
    }
    printf("\n");
    */
    
    pc = start_pc;
    while(pc != end_pc){
        // printf("%llu, %x \n", pc, instructions[pc]);
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
                    printf("VM ERROR: SET INDEX ERROR, plz report this error and send ur code to author through github...");
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
                        
                        accumulator = Object_initInteger((int64_t)(((uint64_t)instructions[pc + 1] << 48) |
                                                                ((uint64_t)instructions[pc + 2] << 32) |
                                                                (instructions[pc + 3] << 16) |
                                                                (instructions[pc + 4])));
                        pc = pc + 5;
                        continue;
                    case CONST_FLOAT: // 64 bit double
                        // free accumulator is necessary
                        Object_free(accumulator);
                    
                        uint64_t integer__ = (uint64_t)(((uint64_t)instructions[pc + 1] << 48) |
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
                
                accumulator = Object_initUserDefinedLambda(param_num, variadic_place, start_pc, copyEnvironment(env), (unsigned char)instructions[pc + 2]);
                pc = pc + jump_steps;
                continue;
            case RETURN:
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
                switch (accumulator->type){
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
                    case BUILTIN_LAMBDA: case VECTOR: case TABLE: case  INTEGER:// builtin lambda or vector or table
                        current_frame_pointer = BUILTIN_PRIMITIVE_PROCEDURE_STACK; // get top frame
                        
                        // save to frame list
                        frames_list[frames_list_length] = current_frame_pointer;
                        frames_list_length++;
                        current_frame_pointer->use_count++; // current frame pointer is used
                        
                        // save to function list
                        functions_list[functions_list_length] = accumulator;
                        functions_list_length++;
                        accumulator->use_count++;
                        
                        pc++;
                        continue;
                    default:
                        printf("ERROR: NEWFRAME error");
                        Object_free(accumulator);
                        accumulator = GLOBAL_NULL;
                    goto VM_END;
                }
            case PUSH_ARG: // push arguments
                accumulator->use_count++; // increase use count
                current_frame_pointer->array[current_frame_pointer->length] = accumulator; // push to env frame
                current_frame_pointer->length++;
                pc++;
                continue;
                
            case CALL:
                // printf("CALL");
                param_num = (0x0FFF & inst);
                v = functions_list[functions_list_length - 1]; // get function
                functions_list[functions_list_length - 1] = NULL; // clear
                functions_list_length--;  // pop that function from list
                v->use_count--; // decrement use count
                
                switch (v->type){
                    case BUILTIN_LAMBDA: // builtin lambda
                        func_ptr = v->data.Builtin_Lambda.func_ptr;
                        accumulator = (*func_ptr)(current_frame_pointer->array, param_num, current_frame_pointer->length - param_num); // call function
                    eval_builtin_lambda:
                        accumulator->use_count++; //必须在pop parameters之前运行这个 eg (car '((x))) 得到了 (x)， 但是如果 accumulator->use_count不加加的话 (x)会被free掉，
                        // 在 pop 完 parameters之后在 decrease accumulator->use_count
                        // pop parameters
                        for(i = 0; i < param_num; i++){
                            temp = current_frame_pointer->array[current_frame_pointer->length - 1];

                            temp->use_count--; // －1 因为在push的时候加1了
                            
                            Object_free(temp); // free object
                            
                            current_frame_pointer->array[current_frame_pointer->length - 1] = NULL; // clear
                            current_frame_pointer->length--; // decrease length
                        }
                        accumulator->use_count--;
                        
                        pc = pc + 1;
                        
                        // free current_frame_pointer
                        free_current_frame_pointer(current_frame_pointer);
                        
                        frames_list_length--; // pop frame list
                        current_frame_pointer = frames_list[frames_list_length - 1];
                    
                        // free lambda
                        Object_free(v);
                        continue;
                    case VECTOR: // vector
                        pc = pc + 1;
                        switch(param_num){
                            case 1: // vector get
                                temp = current_frame_pointer->array[current_frame_pointer->length - 1];
                                integer_ = temp->data.Integer.v; // get index
                                accumulator = v->data.Vector.v[integer_]; // get value
                                
                                temp->use_count--; // pop parameters
                                Object_free(temp);
                                current_frame_pointer->length--; // decrease length
                                
                                // free current_frame_pointer
                                free_current_frame_pointer(current_frame_pointer);
                                
                                frames_list_length--; // pop frame list
                                current_frame_pointer = frames_list[frames_list_length - 1];
                                
                                // free lambda
                                Object_free(v);
                                
                                continue;
                            case 2: // vector set
                                temp = current_frame_pointer->array[current_frame_pointer->length - 2]; // index
                                temp2 = current_frame_pointer->array[current_frame_pointer->length - 1]; // value
                                integer_ = temp->data.Integer.v;
                                
                                // decrease use_count of old_value
                                v->data.Vector.v[integer_]->use_count--;
                                Object_free(v->data.Vector.v[integer_]);
                                
                                // set to vector
                                v->data.Vector.v[integer_] = temp2;
                                temp2->use_count++; // in use
                                
                                // pop parameters
                                for(i = 0; i < param_num; i++){
                                    temp = current_frame_pointer->array[current_frame_pointer->length - 1];
                                    temp->use_count--; // －1 因为在push的时候加1了
                                    Object_free(temp);
                                    
                                    current_frame_pointer->length--; // decrease length
                                }
                                
                                // free current_frame_pointer
                                free_current_frame_pointer(current_frame_pointer);
                                
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
                            case 1: // table get
                                temp = current_frame_pointer->array[current_frame_pointer->length - 1];
                               
                                // get value from table
                                accumulator = Table_getval(v, temp);
                               
                                
                                temp->use_count--; // pop parameters
                                Object_free(temp);
                                current_frame_pointer->length--; // decrease length
                                
                                // free current_frame_pointer
                                free_current_frame_pointer(current_frame_pointer);
                                
                                frames_list_length--; // pop frame list
                                current_frame_pointer = frames_list[frames_list_length - 1];
                                
                                // free lambda
                                Object_free(v);
                                continue;

                            case 2: // object set
                                temp = current_frame_pointer->array[current_frame_pointer->length - 2]; // key
                                temp2 = current_frame_pointer->array[current_frame_pointer->length - 1]; // value
                                
                                Table_setval(v, temp, temp2);
                                
                                // 下面这个不用运行
                                // 因为在 Table_setval的时候会自动增加
                                // temp2->use_count++; // in use
                                
                                // pop parameters
                                for(i = 0; i < param_num; i++){
                                    temp = current_frame_pointer->array[current_frame_pointer->length - 1];
                                    temp->use_count--; // －1 因为在push的时候加1了
                                    Object_free(temp);
                                    
                                    current_frame_pointer->length--; // decrease length
                                }
                                // free current_frame_pointer
                                free_current_frame_pointer(current_frame_pointer);
                                
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
                    case USER_DEFINED_LAMBDA: // user defined function
                    eval_user_defined_lambda:
                        // printf("HERE");
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
                            for(i = param_num; i < required_param_num; i++){
                                current_frame_pointer->array[i] = GLOBAL_NULL;
                                GLOBAL_NULL->use_count++; // 用吗？
                            }
                            current_frame_pointer->length += (required_param_num - param_num); // update length
                        }
                        
                        // save return pc
                        continuation_return_pc[continuation_return_pc_length] = pc+1;
                        continuation_return_pc_length++;
                        // save old env
                        continuation_env[continuation_env_length] = env;
                        continuation_env_length++;
                        
                        // resert pointers
                        env = new_env;
                        pc = start_pc;
                        
                        // free current_frame_pointer
                        free_current_frame_pointer(current_frame_pointer);
                        
                        /* 前面的 copyEnvironmentAndPushFrame 里面的 current_frame_pointer 的 use_count会++, 所以这里得 -- */
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
                                temp = current_frame_pointer->array[current_frame_pointer->length - param_num]; // get first parameter
                                
                                if (vt == NULL) {
                                    printf("ERROR: eval function is only run time supported");
                                    vm_error_jump
                                }
                                Instructions * temp_insts = Insts_init();
                                temp->use_count++;
                                accumulator = compiler_begin(temp_insts,
                                                             temp->type == STRING?
                                                             parser(lexer(temp->data.String.v)) : temp,
                                                             vt,
                                                             NULL,
                                                             NULL,
                                                             1, original_env,
                                                             mt);
                                temp->use_count--;
                                
                                free(temp_insts->array);
                                free(temp_insts);
                                
                                accumulator->use_count++; //必须在pop
                                // pop parameters
                                for(i = 0; i < param_num; i++){
                                    temp = current_frame_pointer->array[current_frame_pointer->length - 1];
                                    
                                    temp->use_count--; // －1 因为在push的时候加1了
                                    
                                    Object_free(temp); // free object
                                    
                                    current_frame_pointer->array[current_frame_pointer->length - 1] = NULL; // clear
                                    current_frame_pointer->length--; // decrease length
                                }
                                accumulator->use_count--;
                                
                                // free current_frame_pointer
                                free_current_frame_pointer(current_frame_pointer);
                                
                                frames_list_length--; // pop frame list
                                current_frame_pointer = frames_list[frames_list_length - 1];
                                
                                // free lambda
                                // this cannot be freed because it is a builtin-function
                                // Object_free(v);
                                continue;
                            case 2: // apply
                                // pc = pc + 1; //不同 +1 因为后面goto后会改变pc
                                v = current_frame_pointer->array[current_frame_pointer->length - param_num]; // get func
                                temp = current_frame_pointer->array[current_frame_pointer->length - param_num + 1];
                                    // get params
                                switch (v->type) {
                                    case BUILTIN_LAMBDA: // builtin lambda
                                                         // push parameters to stack
                                        param_num = 0; // include apply and temp
                                        while (temp != GLOBAL_NULL) {
                                            current_frame_pointer->array[current_frame_pointer->length] = car(temp);
                                            car(temp)->use_count++;
                                            current_frame_pointer->length++;
                                            temp = cdr(temp);
                                            param_num++;
                                        }
                                        accumulator = v->data.Builtin_Lambda.func_ptr(current_frame_pointer->array,
                                                    param_num,
                                                    current_frame_pointer->length - param_num);
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
                                            temp = current_frame_pointer->array[current_frame_pointer->length - 1];
                                            temp->use_count--; // －1 因为在push的时候加1了
                                            Object_free(temp);
                                            
                                            current_frame_pointer->length--; // decrease length
                                        }
                                        // free current_frame_pointer
                                        free_current_frame_pointer(current_frame_pointer);
                                        
                                        current_frame_pointer = temp_frame;
                                        goto eval_user_defined_lambda;
                                    default:
                                        printf("ERROR: Invalid lambda\n");
                                        vm_error_jump
                                }
                                
                            default:
                                printf("ERROR: Invalid Lambda\n");
                                // TODO: Object_free(v)
                                Object_free(accumulator);
                                accumulator = GLOBAL_NULL;
                                goto VM_END;
                        }
                    default:
                        printf("ERROR: Invalid Lambda\n");
                        Object_free(accumulator);
                        accumulator = GLOBAL_NULL;
                        goto VM_END;
                }
            case JMP:
                //printf("JUMP STEPS %d\n", (signed int)((instructions[pc + 1] << 16) | instructions[pc + 2]));
                pc = pc + (signed int)((instructions[pc + 1] << 16) | instructions[pc + 2]);
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
            case SET_TOP: // set to top frame according to index
                // set value and increase length
                env->frames[env->length-1]->array[instructions[pc+1]] = accumulator;
                env->frames[env->length-1]->length++;

                accumulator->use_count++; // increase use_count
                pc+=2;
                accumulator = GLOBAL_NULL;
                continue;
            // tail call push parameters
            // opcode index
            case TAIL_CALL_PUSH:
                env->frames[env->length-1]->array[instructions[pc] & 0x0FFF] = accumulator;
                // 不用增加use count因为最后会被free掉
                pc++;
                accumulator = GLOBAL_NULL; // 必须set为global null， 要不然会出错， 因为GET的时候会free掉accumulator, 而此时的accumulator的use count正好是0
                continue;
            default:
                printf("ERROR: Invalid opcode %d\n", opcode);
                vm_error_jump
        }
    }
    
VM_END:
    // free BUILTIN_PRIMITIVE_PROCEDURE_STACK
    accumulator->use_count+=1;
    
    
    BUILTIN_PRIMITIVE_PROCEDURE_STACK->use_count--;
    EF_free(BUILTIN_PRIMITIVE_PROCEDURE_STACK);
    
    accumulator->use_count-=1;

    return accumulator;
}

























#endif
