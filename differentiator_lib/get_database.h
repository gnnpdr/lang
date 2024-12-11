#ifndef _BASE_H_
#define _BASE_H_ 

#include "draw_tree.h"

#include <sys/stat.h>

static const size_t MAX_STRS_AMT  =  50;
static const size_t MAX_FILE_SIZE = 3000;

struct Input 
{
    char* name;
    char* text;
    size_t size;
    
    size_t node_amt;
};

void input_ctor (Input *const base_text, Err_param *const error);
void get_database_name(Input *const base_text, char **const argv, Err_param *const error);
void get_database_text (Input *const base_text, Err_param *const error);
void input_dtor(Input* base_text);

#endif //_BASE_H_