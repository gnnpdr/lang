#include <stdio.h>

#include "lex_analysis.h"

static void make_token(Token *const tokens, Type type, int val, ErrList *const list);
static int find_free_token(Token *const tokens, ErrList *const list);

static size_t make_id(Id *const ids, size_t len, char *const text, ErrList *const list);

static void get_num(char *const text, size_t *const pointer, Token *const tokens, ErrList *const list);
static void get_op(char *const text, size_t *const pointer, Token *const tokens, ErrList *const list);
static size_t find_match(char *const start_address, size_t len);
static size_t find_id(Id *const ids, size_t len, char *const text, ErrList *const list);
static void get_id(char *const text, Id *const ids, size_t *const pointer, Token *const tokens, ErrList *const list);

void lex_analysis(Token *const tokens, Id *const ids, Input *const base_text, ErrList *const list)
{
    assert(tokens);
    assert(ids);
    assert(base_text);
    assert(list);

    char* text = base_text->text;

    size_t pointer = 0;
    size_t size = base_text->size;
    char symb = text[pointer];

    while (pointer != size)
    {
        symb = text[pointer];
        while (isspace(symb))
        {
            pointer++;
            symb = text[pointer];
        }

        if (symb >= '0' && symb <= '9')
        {
            get_num(text, &pointer, tokens, list);
        }
        else if (symb == ID_MARK)
        {
            get_id(text, ids, &pointer, tokens, list);
        }
        else
        {
            get_op(text, &pointer, tokens, list);
        }
        RETURN_VOID
    }

   /*printf("TOKENS\n");
    for (int i = 0; i < TOKEN_AMT; i++)
    {
        printf("num %d, type %d, value %d\n", i, tokens[i].type, tokens[i].value);
    }
    printf("---------------\n");

    printf("IDS\n");
    for (int i = 0; i < ID_AMT; i++)
    {
        printf("num %d, str %.10s, len %d\n", i, ids[i].start_address, ids[i].len);
    }
    printf("---------------\n");*/
}

//------------------TOKENS------------------------------
Token* tokens_ctor(ErrList *const list)
{
    assert(list);

    Token* tokens = (Token*)calloc(TOKEN_AMT, sizeof(Token));
    ALLOCATION_CHECK_PTR(tokens)

    for (size_t i = 0; i < TOKEN_AMT; i++)
        tokens[i].value = ERROR_VALUE_INT;

    return tokens;
}

void make_token(Token *const tokens, Type type, int val, ErrList *const list)
{
    assert(tokens);
    assert(list);

    size_t ind = find_free_token(tokens, list);
    RETURN_VOID

    tokens[ind].type = type;
    tokens[ind].value = val;
}

int find_free_token(Token *const tokens, ErrList *const list)
{
    assert(tokens);
    assert(list);

    size_t free_ind = ERROR_VALUE_SIZE_T;

    for (size_t ind = 0; ind < TOKEN_AMT; ind++)
    {
        if (tokens[ind].value == ERROR_VALUE_INT)
        {
            free_ind = ind;
            break;
        }
    }

    if (free_ind == ERROR_VALUE_SIZE_T)
        ERROR(ALLOCATION_ERROR)

    return free_ind;
}

void tokens_dtor(Token *const tokens)
{
    free(tokens);
}

//-------------------------IDS--------------------------------
Id* id_ctor(ErrList *const list)
{
    assert(list);

    Id* ids = (Id*)calloc(ID_AMT, sizeof(Id));
    ALLOCATION_CHECK_PTR(ids)

    for (int i = 0; i < ID_AMT; i++)
        ids[i].len = ERROR_VALUE_SIZE_T;

    return ids;
}

size_t make_id(Id *const ids, size_t len, char *const text, ErrList *const list)
{
    assert(ids);
    assert(text);
    assert(list);

    size_t ind = find_id(ids, len, text, list);
    RETURN_SIZE_T

    ids[ind].len = len;
    ids[ind].start_address = text;

    return ind;
}

size_t find_id(Id *const ids, size_t len, char *const text, ErrList *const list)
{
    assert(ids);
    assert(list);
    assert(text);

    for (size_t i = 0; i < ID_AMT; i++)
    {
        if (ids[i].len == ERROR_VALUE_SIZE_T)
            return i;

        int cmp_res = strncmp(ids[i].start_address, text, len);

        if (cmp_res == 0)
            return i;
    }

    return ERROR_VALUE_SIZE_T;
}

void ids_dtor(Id *const ids)
{
    free(ids);
}

//----------------------LEX_ANALYSIS-------------------------
void get_num(char *const text, size_t *const pointer, Token *const tokens, ErrList *const list)
{
    assert(text);
    assert(pointer);
    assert(tokens);
    assert(list);

    double val = 0;

    size_t start_pointer = *pointer;
    while (text[*pointer] >= '0' && text[*pointer] <= '9')
    {
        val = val * 10 + text[*pointer] - '0';
        (*pointer)++;
    }

    if (*pointer == start_pointer)  //макросом эту ошибку надо
    {
        ERROR(SYN_ERROR)
        return;
    }

    make_token(tokens, NUM, val, list);
    RETURN_VOID
}

void get_op(char *const text, size_t *const pointer, Token *const tokens, ErrList *const list)
{
    assert(text);
    assert(pointer);
    assert(tokens);
    assert(list);

    size_t len = 0;
    char* start_address = text + *pointer;
    
    while (isgraph(*(start_address + len)))  //если операция самодельная - она ведь может и цифры содержать или @
        len++;

    char op_start = text[*pointer];

    /*if (len == 0)
    {
        if (op_start == END_MARK)
            len = 5;
        else 
            len = 1;
    }*/

    int op_ind = find_match(start_address, len);
    if (op_ind == ERROR_VALUE_SIZE_T)
    {
        if (op_start == END_MARK || isdigit(op_start) ) 
            return;
    }
    
    make_token(tokens, OP, op_ind, list);
    RETURN_VOID

    *pointer += len;
}

size_t find_match(char *const start_address, size_t len)
{
    assert(start_address);

    size_t match_ind = ERROR_VALUE_SIZE_T;

    for (size_t ind = 0; ind < OP_AMT; ind++)
    {
        int cmp_res = strncmp(start_address, operations[ind]->name, len);
        if(cmp_res == 0)
        {
            match_ind = ind;
            break;
        }
    }

    return match_ind;
}

void get_id(char *const text, Id *const ids, size_t *const pointer, Token *const tokens, ErrList *const list)
{
    assert(text);
    assert(pointer);
    assert(tokens);
    assert(list);

    (*pointer)++;

    size_t len = 0;
    char* start_address = text + *pointer;

    while (isalpha(*(start_address + len)))
        len++;
    
    int id_ind = make_id(ids, len, start_address, list);
    RETURN_VOID

    make_token(tokens, ID, id_ind, list);
    RETURN_VOID

    *pointer += len;
}