#include <stdio.h>

#include "lex_analysis.h"

//и еще - хотелось бы, чтобы концевой эдемент начинался с рубля, а он не читается. почему?
//проблемы - не читается концевой элемент  (он будто конец и число)
//----------готово
//айди ситаются разными. Надо вставить проверку на наличие таких 

static void make_token(Token *const tokens, Type type, double val, Err_param *const error);
static int find_free_token(Token *const tokens, Err_param *const error);

static size_t make_id(Id *const ids, size_t len, char *const text, Err_param *const error);

static void get_num(char *const text, size_t *const pointer, Token *const tokens, Err_param *const error);
static void get_op(char *const text, size_t *const pointer, Token *const tokens, Err_param *const error);
static size_t find_match(char *const start_address, size_t len);
static size_t find_id(Id *const ids, size_t len, char *const text, Err_param *const error);
static void get_id(char *const text, Id *const ids, size_t *const pointer, Token *const tokens, Err_param *const error);

void lex_analysis(Token *const tokens, Id *const ids, Input *const base_text, Err_param* error)
{
    assert(base_text);
    assert(error);

    char* text = base_text->text;

    size_t pointer = 0;
    size_t size = base_text->size;
    //printf("SIZE %d\n", size);
    char symb = text[pointer];

    while (pointer != size)
    {
        symb = text[pointer];
        while (isspace(symb))
        {
            pointer++;
            symb = text[pointer];
        }
        //printf("cur pointer %d\n", pointer);
        //printf("CUR TEXT\n--------\n%s\n-----\n", text + pointer);

        if (symb >= '0' && symb <= '9')
        {
            //printf("GET NUM\n");
            get_num(text, &pointer, tokens, error);
        }
        else if (symb == ID_MARK)
        {
            //printf("GET ID\n");
            get_id(text, ids, &pointer, tokens, error);
        }
        else
        {
            //printf("GET OP\n");
            get_op(text, &pointer, tokens, error);
        }
    }

    printf("TOKENS\n");
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
    printf("---------------\n");
}

//------------------TOKENS------------------------------
Token* tokens_ctor(Err_param *const error)
{
    assert(error);

    Token* tokens = (Token*)calloc(TOKEN_AMT, sizeof(Token));
    ALLOCATION_CHECK_RET(tokens)

    for (int i = 0; i < TOKEN_AMT; i++)
        tokens[i].value = ERROR_VALUE;

    return tokens;
}

void make_token(Token *const tokens, Type type, double val, Err_param *const error)
{
    assert(tokens);
    assert(error);

    size_t ind = find_free_token(tokens, error);
    RETURN_VOID

    tokens[ind].type = type;
    tokens[ind].value = val;
}

int find_free_token(Token *const tokens, Err_param *const error)
{
    assert(tokens);
    assert(error);

    size_t free_ind = ERROR_VALUE_SIZE_T;

    for (size_t ind = 0; ind < TOKEN_AMT; ind++)
    {
        if (tokens[ind].value == ERROR_VALUE)
        {
            free_ind = ind;
            break;
        }
    }

    if (free_ind == ERROR_VALUE_SIZE_T)
    {
        printf("no free tokens\n");
        ERROR(ALLOCATION_ERROR)
    }

    return free_ind;
}

void tokens_dtor(Token *const tokens)
{
    free(tokens);
}

//-------------------------IDS--------------------------------
Id* id_ctor(Err_param *const error)
{
    assert(error);

    Id* ids = (Id*)calloc(ID_AMT, sizeof(Id));
    ALLOCATION_CHECK_RET(ids)

    for (int i = 0; i < ID_AMT; i++)
        ids[i].len = ERROR_VALUE_SIZE_T;

    return ids;
}

size_t make_id(Id *const ids, size_t len, char *const text, Err_param *const error)
{
    assert(ids);
    assert(text);
    assert(error);

    size_t ind = find_id(ids, len, text, error);
    if (ind == ERROR_VALUE_SIZE_T)
    {
        printf("no free ids\n");
        return ind;
    }

    ids[ind].len = len;
    ids[ind].start_address = text;

    return ind;
}

size_t find_id(Id *const ids, size_t len, char *const text, Err_param *const error)
{
    assert(ids);
    assert(error);
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
void get_num(char *const text, size_t *const pointer, Token *const tokens, Err_param *const error)
{
    assert(text);
    assert(pointer);
    assert(tokens);
    assert(error);

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

    make_token(tokens, NUM, val, error);
    RETURN_VOID
}

void get_op(char *const text, size_t *const pointer, Token *const tokens, Err_param *const error)
{
    assert(text);
    assert(pointer);
    assert(tokens);
    assert(error);

    size_t len = 0;
    char* start_address = text + *pointer;
    
    while (isalpha(*(start_address + len)))
        len++;

    char op_mark = text[*pointer];

    if (len == 0)
    {
        if (op_mark == END_MARK)//вот такой вот костыль, но что уж
            len = 5;
        else 
            len = 1;
    }

    size_t op_ind = find_match(start_address, len);
    if (op_ind == ERROR_VALUE_SIZE_T)
        return;
    
    make_token(tokens, OP, op_ind, error);

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
            //printf("def op %s\n", operations[ind]->name);
            match_ind = ind;
            break;
        }
    }

    return match_ind;
}

void get_id(char *const text, Id *const ids, size_t *const pointer, Token *const tokens, Err_param *const error)
{
    assert(text);
    assert(pointer);
    assert(tokens);
    assert(error);

    (*pointer)++;

    size_t len = 0;
    char* start_address = text + *pointer;

    while (isalpha(*(start_address + len)))
        len++;
    
    size_t id_ind = make_id(ids, len, start_address, error);
    RETURN_VOID

    make_token(tokens, ID, id_ind, error);
    RETURN_VOID

    *pointer += len;
}