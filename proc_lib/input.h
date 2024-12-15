#ifndef _INPUT_H_
#define _INPUT_H_

#include "..\code_gen\code_gen.h"

enum WordType
{
    CMD, 
    ARG,
    LABEL_ARG,
    LABEL_CMD
};

struct Word
{
    char* word_start;
    size_t len;
    size_t str_num; 
    WordType type;
};

static const size_t FILE_CMD_AMT = 100;

Word* word_list_ctor(ErrList *const list);
void word_list_dtor(Word *const words);

void handle_text_wname (Input *const text, const char *const name, ErrList *const list);
void get_code(Input *const asm_text, Word *const words, ErrList *const list);

#endif //_INPUT_H_