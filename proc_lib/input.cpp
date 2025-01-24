#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "input.h"

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

    char* text = asm_text->text;

    size_t size = asm_text->size;
    size_t str_num = 0;
    size_t len = 1;
    size_t word_num = 0;

    for (size_t pointer = 0; pointer < size; pointer++)
    {
        if (!isspace(text[pointer]))
        {
            if (text[pointer] == '\0')
                continue;

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
            else
            {
                if (word_num == 0)
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
            if (text[pointer] == '\n' )
                str_num++;
            continue;
        }
    }
    printf("WORDS!\n");
    for (int i = 0; i < word_num; i++)
        printf("text %.5s\nlen %d\nstr %d\ntype %d\n----------\n", words[i].word_start, words[i].len, words[i].str_num, words[i].type);
    printf("WORD END\n\n");
}

void get_bin_code(Input *const base_text, Proc *const proc, ErrList *const list)
{
    assert(base_text);
    assert(list);

    char* base_file_name = base_text->name;
    FILE* input_file;

    input_file = fopen(base_file_name, "r");
    FILE_CHECK(input_file)

    char* text = base_text->text;
    int* code = proc->code;

    size_t size = 0;

    count_file_size(base_text->name, &size, list);
    RETURN_VOID

    size_t dig = 0;
    size_t num_len = 0;

    int num = 0;

    for (int ind = 0; ind < size; ind++)
    {
        if (isspace(text[ind]) || text[ind] == '\0')
            continue;
            
        while (isdigit(text[ind]))
        {
            num = num * 10 + text[ind] - '0';
            num_len++;
            ind++;
        }

        code[dig] = num;
        num_len = 0;
        num = 0;

        dig++;
    }

    int close_res = fclose(input_file);
    CLOSE_CHECK

    /*for (int i = 0; i < dig; i++)
        printf("%d ", code[i]);*/

    //printf("few\n");
    
    proc->size = dig;
}