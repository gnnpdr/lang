#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "input.h"

//надо сделать так, чтобы не читало нули в конце

Word* word_list_ctor(ErrList *const list)
{
    assert(list);

    Word* words = (Word*)calloc(FILE_CMD_AMT, sizeof(Word));
    ALLOCATION_CHECK_PTR(words)

    for (size_t i = 0; i < FILE_CMD_AMT; i++)
        words[i].len = ERROR_VALUE_SIZE_T;

    return words;
}

void word_list_dtor(Word *const words)
{
    free(words);
}

void handle_text_wname (Input *const text, const char *const name, ErrList *const list)
{
    assert(text);
    assert(list);

    strncpy(text->name, name, MAX_STR_LEN);
    CPY_CHECK(text->name)
    get_database_text(text, list);
    RETURN_VOID
}

void get_code(Input *const asm_text, Word *const words, ErrList *const list)
{
    assert(asm_text);
    assert(words);
    assert(list);

    char* text = asm_text->text;  //он уже загружен другой функцией в мэйне

    size_t size = asm_text->size;
    size_t str_num = 0;
    size_t len = 1;
    size_t word_num = 0;

    for (size_t pointer = 0; pointer < size; pointer++)
    {
        if (!isspace(text[pointer]))
        {
            words[word_num].word_start = text + pointer;
            pointer++;

            while(!isspace(text[pointer]))
            {
                len++;
                pointer++;
            }
            words[word_num].len = len;
            words[word_num].str_num = str_num;
            if (str_num != 0)
            {
                if (words[word_num - 1].str_num < str_num)
                    words[word_num].type = CMD;
                else
                    words[word_num].type = ARG;
            }

            if (text[pointer] == '\n')
                str_num++;

            len = 1;
            word_num++;
        }
        else
        {
            if (text[pointer] == '\n')
                str_num++;
            continue;
        }
            
    }

    for (int i = 0; i < word_num; i++)
        printf("text %.10s\nlen %d\nstr %d\ntype %d\n----------\n", words[i].word_start, words[i].len, words[i].str_num, words[i].type);
}