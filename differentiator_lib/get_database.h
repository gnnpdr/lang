#ifndef _BASE_H_
#define _BASE_H_ 

#include "tree_structure.h"

#include <sys/stat.h>
#include <string.h>

static const size_t MAX_STRS_AMT  =  50;
static const size_t MAX_FILE_SIZE = 3000;
static const size_t MAX_STR_LEN   = 100;
static const size_t MAX_CMD_AMT   = 150;

struct Input 
{
    char* name;
    char* text;
    size_t size;
};

void input_ctor (Input *const base_text, ErrList *const list);
void get_text(Input *const base_text, char **const argv, ErrList *const list);
void get_database_name(Input *const base_text, char **const argv, ErrList *const list);
void get_database_text (Input *const base_text, ErrList *const list);
void input_dtor(Input* base_text);
void count_file_size(const char *const name, size_t* size, ErrList *const list);

#endif //_BASE_H_