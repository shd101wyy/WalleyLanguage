//
//  all_functions_header.h
//  walley
//
//  Created by WangYiyi on 6/10/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#ifndef walley_all_functions_header_h
#define walley_all_functions_header_h
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>

typedef struct Data Data;


// integer
Data * builtin_integer_add(Data * self, Data ** params, uint16_t param_num);
Data * builtin_integer_sub(Data * self, Data ** params, uint16_t param_num);
Data * builtin_integer_mul(Data * self, Data ** params, uint16_t param_num);
Data * builtin_integer_div(Data * self, Data ** params, uint16_t param_num);
Data * Data_initInteger();
Data * Integer_initInstance(int64_t v);

// float
Data * builtin_float_add(Data * self, Data ** params, uint16_t param_num);
Data * builtin_float_sub(Data * self, Data ** params, uint16_t param_num);
Data * builtin_float_mul(Data * self, Data ** params, uint16_t param_num);
Data * builtin_float_div(Data * self, Data ** params, uint16_t param_num);
Data * Data_initFloat();
Data * Float_initInstance(double v);


// builtin fn
Data * Data_initBuiltinFn();
Data * BuiltinFn_initInstance(Data* (*func_ptr)(Data*, Data **, uint16_t));


// string
Data * Data_initString();
Data * String_initInstance(char * v);

#endif
