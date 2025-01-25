#ifndef _LEX_H_
#define _LEX_H_

#include <ctype.h>
#include <math.h>

#include "..\differentiator_lib\get_database.h"

static const size_t TOKEN_AMT = 150;
static const size_t ID_AMT = 15;

enum IdType
{
    VAR_ID,
    FUNC_ID
};


struct Token
{
    Type type;
    int value;
};

static const size_t ARGS_AMT = 10;

struct Id
{
    char* start_address;
    size_t len;
    //доп инфа
    
    IdType type;
    size_t var_amt; //в случае функции, считаем, что последняя переменная - возвращаемое значение
    size_t* args;
};

Token* tokens_ctor(ErrList *const list);
Id* id_ctor(ErrList *const list);

void tokens_dtor(Token *const tokens);
void ids_dtor(Id *const ids);

void lex_analysis(Token *const tokens, Id *const ids, Input *const base_text, ErrList *const list);

#endif //_LEX_H_