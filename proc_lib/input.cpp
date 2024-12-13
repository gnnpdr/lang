#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "input.h"

static void remove_carriage(Input *const text, ErrList *const list);

void handle_text_argv (Input *const text, char **const argv, ErrList *const list)
{
    assert(text);
    assert(argv);
    assert(list);

    get_database_name(text, argv, list);  
    RETURN_VOID
    get_database_text(text, list);
    RETURN_VOID
    remove_carriage(text, list);
    RETURN_VOID
}

void handle_text_wname (Input *const text, const char *const name, ErrList *const list)
{
    assert(text);
    assert(list);

    strncpy(text->name, name, MAX_STR_LEN);
    CPY_CHECK(text->name)
    get_database_text(text, list);
    RETURN_VOID
    remove_carriage(text, list);
    RETURN_VOID
}

void remove_carriage(Input *const text, ErrList *const list)
{
    assert(text);
    assert(list);

    size_t symb_num = 0;
    size_t word_cnt = 0;

    size_t size = text->size;
    char* buf = text->text;
    char** addresses = text->addresses;

    addresses[word_cnt] = buf + symb_num;

    while (symb_num < size)
    {
        char* ch = buf + symb_num;

        if (*ch == SPACE)
        {
            *ch = '\0';
            word_cnt++;
            addresses[word_cnt] = buf + symb_num + 1;  //было +1. Можно поменять на +2 чтобы не было проблем в нахождении : в лэйблах
        }
        else if (*ch == '\r')
        {
            *ch = '\0';
        }
        else if (*ch == COMMENT_MARK)
        {
            do
            {
                symb_num++;
                ch = buf + symb_num;
            }
            while (*ch != '\n');
        }
        else if (*ch == '\n')
        {
            *ch = '\0';
            word_cnt++;
            addresses[word_cnt] = buf + symb_num + 1;
        }

        symb_num++;
    }

    text->cmd_amt = word_cnt;
}