#include <stdio.h>

#include "lex_analysis.h"

static void get_var(char *const text, Id *const ids, size_t *const pointer, Token *const tokens, ErrList *const list);
static void get_num(char *const text, size_t *const pointer, Token *const tokens, ErrList *const list);
static void get_op(char *const text, Id *const ids, size_t *const pointer, Token *const tokens, ErrList *const list);

static void make_token(Token *const tokens, Type type, int val, ErrList *const list);
static int find_free_token(Token *const tokens, ErrList *const list);

static size_t make_id_var(Id *const ids, size_t len, char *const text, ErrList *const list);
static size_t make_id_func(Id *const ids, size_t len, char *const text, size_t* pointer, ErrList *const list);
static void fill_func_args (size_t func_ind, Id *const ids, char* text, size_t* pointer, ErrList *const list);

static size_t find_match_op(char *const start_address, size_t len);
static size_t find_id(Id *const ids, size_t len, char *const text, ErrList *const list);
static size_t find_free_arg (Id id);

//--------COMMON--------------------------
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

    while (pointer <= size)
    {
        symb = text[pointer];
        while (isspace(symb))
        {
            pointer++;
            symb = text[pointer];
        }

        if (symb >= '0' && symb <= '9' || symb == '-')
            get_num(text, &pointer, tokens, list);
        else if (symb == ID_MARK)
            get_var(text, ids, &pointer, tokens, list);
        else
            get_op(text, ids, &pointer, tokens, list);

        RETURN_VOID
    }
}

//-----------NUM------------------------

void get_num(char *const text, size_t *const pointer, Token *const tokens, ErrList *const list)
{
    assert(text);
    assert(pointer);
    assert(tokens);
    assert(list);

    int val = 0;

    size_t start_pointer = *pointer;
    bool is_negative = false;

    if (text[*pointer] == '-')
    {
        is_negative = true;
        (*pointer)++;
    }
        
    while (text[*pointer] >= '0' && text[*pointer] <= '9')
    {
        val = val * 10 + text[*pointer] - '0';
        (*pointer)++;
    }

    if (is_negative)
        val = val * (-1);

    if (*pointer == start_pointer)
    {
        ERROR(SYN_ERROR)
        return;
    }

    make_token(tokens, NUM, val, list);
    RETURN_VOID
}

//-------------------------IDS--------------------------------

//--------common------------------

Id* id_ctor(ErrList *const list)
{
    assert(list);

    Id* ids = (Id*)calloc(ID_AMT, sizeof(Id));
    ALLOCATION_CHECK_PTR(ids)

    for (int i = 0; i < ID_AMT; i++)
        ids[i].len = ERROR_VALUE_SIZE_T;
        
    return ids;
}

void ids_dtor(Id *const ids)
{
    free(ids);
}

//---------var--------------------

void get_var(char *const text, Id *const ids, size_t *const pointer, Token *const tokens, ErrList *const list)
{
    assert(text);
    assert(pointer);
    assert(tokens);
    assert(list);

    (*pointer)++;

    size_t len = 0;
    char* start_address = text + *pointer;

    while (!isspace(*(start_address + len)) && *(start_address + len) != '.')
        len++;
    
    int id_ind = make_id_var(ids, len, start_address, list);
    RETURN_VOID

    make_token(tokens, ID, id_ind, list);
    RETURN_VOID

    *pointer += len;
}

size_t find_id(Id *const ids, size_t id_len, char *const id_start, ErrList *const list)
{
    assert(ids);
    assert(list);
    assert(id_start);

    for (size_t i = 0; i < ID_AMT; i++)
    {
        if (ids[i].len == ERROR_VALUE_SIZE_T)
        {
            return i;
        }

        int cmp_res = strncmp(ids[i].start_address, id_start, id_len);
        if (cmp_res == 0)
            return i;
    }

    printf("no matches or free places\n");
    ERROR(ALLOCATION_ERROR)
    return ERROR_VALUE_SIZE_T;
}

size_t make_id_var(Id *const ids, size_t id_len, char *const id_start, ErrList *const list)
{
    assert(ids);
    assert(id_start);
    assert(list);

    size_t ind = find_id(ids, id_len, id_start, list);
    RETURN_SIZE_T

    if (ids[ind].len == ERROR_VALUE_SIZE_T)
    {
        ids[ind].type = VAR_ID;
        ids[ind].len = id_len;
        ids[ind].start_address = id_start;
    }
    
    return ind;
}

//-------------funcs------------------

size_t make_id_func(Id *const ids, size_t len, char *const text, size_t* pointer, ErrList *const list)
{
    assert(ids);
    assert(text);
    assert(list);
    assert(pointer);

    size_t ind = find_id(ids, len, text + *pointer, list);
    RETURN_SIZE_T

    if (ids[ind].len == ERROR_VALUE_SIZE_T)
    {
        ids[ind].type = FUNC_ID;
        ids[ind].len = len;
        ids[ind].start_address = text + *pointer;
    }

    *pointer += len;

    return ind;
}

//-------------------OP--------------------------------

void get_op(char *const text, Id *const ids, size_t *const pointer, Token *const tokens, ErrList *const list)
{
    assert(text);
    assert(pointer);
    assert(tokens);
    assert(list);

    size_t len = 0;
    char* start_address = text + *pointer;
    
    while (!isspace(*(start_address + len)))
        len++;

    char op_start = text[*pointer];

    int op_ind = find_match_op(start_address, len);

    if (op_ind == ERROR_VALUE_SIZE_T)
    {
        op_ind = make_id_func(ids, len, text, pointer, list);
        make_token(tokens, ID, op_ind, list);
        RETURN_VOID 
        return;
    }
        
    make_token(tokens, OP, op_ind, list);
    RETURN_VOID
    *pointer += len;
}

size_t find_match_op(char *const start_address, size_t len)
{
    assert(start_address);

    size_t match_ind = ERROR_VALUE_SIZE_T;

    for (size_t ind = 0; ind < OP_AMT; ind++)
    {
        int cmp_res = strncmp(start_address, operations[ind]->name, len);
        //printf("-------------\nCMB %s\nTEXT %.7s\nLEN %d\n", operations[ind]->name, start_address, len);
        if(cmp_res == 0)
        {
            match_ind = ind;
            break;
        }
    }

    return match_ind;
}

//------------------TOKENS------------------------------

//---------common-------------------

Token* tokens_ctor(ErrList *const list)
{
    assert(list);

    Token* tokens = (Token*)calloc(TOKEN_AMT, sizeof(Token));
    ALLOCATION_CHECK_PTR(tokens)

    for (size_t i = 0; i < TOKEN_AMT; i++)
        tokens[i].value = ERROR_VALUE_INT;

    return tokens;
}

void tokens_dtor(Token *const tokens)
{
    free(tokens);
}

//----------make new-----------------

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

void make_token(Token *const tokens, Type type, int val, ErrList *const list)
{
    assert(tokens);
    assert(list);

    size_t ind = find_free_token(tokens, list);
    RETURN_VOID

    tokens[ind].type = type;
    tokens[ind].value = val;
}