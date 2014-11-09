// WalleyLanguage.cpp : Defines the entry point for the console application.
//
/* // IF windows, uncomment this
#include "stdafx.h"
*/
#include "walley.h"

int main(int argc, char *argv[]){

    signal(SIGINT, quit_program_signal_handler); // register sigint signal handler
    
    srand((unsigned int)time(NULL)); // set seed
    //printf("Current Working Directory %s\n", getcwd(NULL, 0));

    // ######################################################
    // ######################################################
    // ######################################################
    // ######################################################
    // ######################################################
    uint16_t i;
    // init sys_argv
    SYS_ARGV = Object_initVector(0, argc);
    SYS_ARGV->data.Vector.length = argc;
    SYS_ARGV->use_count = 1; // in use
    for (i = 0; i < argc; i++){
        vector_Set(SYS_ARGV, i, Object_initString(argv[i], strlen(argv[i])));
    }

    // ######################################################
    // ######################################################
    // ######################################################
    // ######################################################
    //Walley_Run_File("/Users/wangyiyi/百度云同步盘/Github/Walley0.3/walley/WalleyLanguage/tutorial.wa");
    //number_debug(Object_initRatio(-6, 15));
    // Walley_Run_File("/Users/wangyiyi/∞Ÿ∂»‘∆Õ¨≤Ω≈Ã/Github/Walley0.3/walley/walley/tutorial.wa");
    //return 0;
    if (argc == 1) {
        printf("\nWalley Language 0.3.8493\n");
#ifdef EMSCRIPTEN
        printf("Porting to JavaScript using Emscripten 1.25.0\n");
#endif
        printf("Copyright (c) 2012-2014 Yiyi Wang\n");
        printf("All Rights Reserved\n");
        printf("Press ctrl-c to quit the program\n\n");
        Walley_Repl();
        return 0;
    }
    // run file
    else if (argc == 2){
        //char abs_path[256];
        //printf("Absolute path %s\n", realpath(argv[1], abs_path));
        Walley_Run_File(argv[1]);
    }
    else if (argc == 3 && str_eq(argv[1], "compile")){ // run file
        Walley_Compile(argv[2]);
    }
    return 0;
}














