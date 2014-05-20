//
//  main.c
//  walley
//
//  Created by WangYiyi on 4/27/14.
//  Copyright (c) 2014 WangYiyi. All rights reserved.
//

#include <stdio.h>
#include "walley.h"

int main(int argc, char *argv[]){
    srand((unsigned int)time(NULL)); // set seed

    // ######################################################
    // ######################################################
    // ######################################################
    // ######################################################
    // ######################################################
    int i;
    // init sys_argv
    SYS_ARGV = Object_initVector(0, argc);
    SYS_ARGV->data.Vector.length = argc;
    SYS_ARGV->use_count = 1; // in use
    for(i = 0; i < argc; i++){
        vector_Set(SYS_ARGV, i, Object_initString(argv[i], strlen(argv[i])));
    }
    // ######################################################
    // ######################################################
    // ######################################################
    // ######################################################
    
    //number_debug(Object_initRatio(-6, 15));
    // Walley_Run_File("/Users/wangyiyi/百度云同步盘/Github/Walley0.3/walley/walley/tutorial.wa");
    //return 0;
    if (argc == 1) {
        printf("\nWalley Language 0.3.8465\n");
        printf("Copyright (c) 2012-2014 Yiyi Wang\n");
        printf("All Rights Reserved\n\n");
        Walley_Repl();
        return 0;
    }
    // run file
    else if (argc == 2){
        Walley_Run_File(argv[1]);
    }
    else if (argc == 3 && str_eq(argv[1], "compile")){ // run file
        Walley_Compile(argv[2]);
    }
    return 0;
}














