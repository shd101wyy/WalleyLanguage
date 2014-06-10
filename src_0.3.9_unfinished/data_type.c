#ifndef DATA_TYPE
#define DATA_TYPE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>

typedef struct Object Object;
typedef struct P_Value P_Value;
struct Object {
P_Value * msgs;
P_Value * actions;
uint32_t size;
uint32_t length;
};


#endif

