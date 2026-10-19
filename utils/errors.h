#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// in case an error appears due to a '\n' pass 1 postion before it so it displays the correct line
void disp_error(const char *source, int line , int col , int position , int err_code , const char *messgae , ... ){
    fprintf( stderr ,"Error at line %d , Column %d\n" , line , col);
    va_list msg;
    va_start(msg , messgae);
    vfprintf(stderr , messgae , msg);
    fprintf(stderr , "\n");
    va_end(msg);
    int err_pos = position;
    if(position){
        position--;
    }
    while(source[position] != '\n' && position != 0){
        position--;
    }
    if(source[position] == '\n'){
        position++;
    }
    int num_of_tabs = 0;
    while(source[position] != '\n' && source[position] != '\0' && source[position] != '\r'){
        if(source[position] == '\t'  && err_pos > position){
            num_of_tabs++;
        }
        fprintf(stderr ,"%c" , source[position]);
        position++;
    }
    fprintf(stderr,"\n");
    int i = 1;
    int j = 0;
    while(j < num_of_tabs){
        fprintf(stderr,"\t");
        j++;
    }
    while(i < col - num_of_tabs){
        fprintf(stderr," ");
        i++;
    }
    fprintf(stderr,"^\n");
    exit(err_code);
}