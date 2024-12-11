#include <stdio.h>

#include "syn_analysis.h"

static bool op_match(Token token, const OperationParameters *const arr, size_t arr_size);

static Node* get_expression(Token *const tokens, size_t *const pointer, Err_param *const error);
static Node* get_t (Token *const tokens, size_t *const pointer, Err_param *const error);
static Node* get_brace (Token *const tokens, size_t* pointer, Err_param *const error);
static Node* get_unary(Token *const tokens, size_t *const pointer, Err_param *const error);
static Node* get_binary(Token *const tokens, size_t *const pointer, Err_param *const error);

static Node* get_num(Token *const token, size_t *const pointer, Err_param *const error);
static Node* get_id(Token *const token, size_t *const pointer, Err_param *const error);

Node* syn_analysis(Token *const tokens, Err_param *const error)
{
    assert(tokens);
    assert(error);

    size_t pointer = 0;

    Node* root = get_operation(tokens, &pointer, error);

    if (tokens[pointer].type != OP && tokens[pointer].value != END)
    {
        printf("ERROR\n");
        ERROR(SYN_ERROR)
        return nullptr;
    }

    pointer++;

    return root;
}

Node* get_operation(Token *const tokens, size_t *const pointer, Err_param *const error)
{
    assert(tokens);
    assert(pointer);
    assert(error);

    Node* val = nullptr;

    if (tokens[*pointer].type == OP)
    {
        if (tokens[*pointer].value == BR)
        {;
            (*pointer)++;

            size_t start_pointer = *pointer;

            val = get_expression(tokens, pointer, error);  //здесь могут возникнуть проблемы?

            if (tokens[*pointer].type != OP && tokens[*pointer].value != BR)
            {
                ERROR(SYN_ERROR)
                return nullptr;
            }

            (*pointer)++;
        }
        else if (tokens[*pointer].value == IF)
        {
            (*pointer)++;
            val = get_if(tokens, pointer, error);
        }
    }
    else
    {
        val = get_equality(tokens, pointer, error);
    }
    
    return val;
}

Node* get_if(Token *const tokens, size_t *const pointer, Err_param *const error)
{
    assert(tokens);
    assert(pointer);
    assert(error);

    if (tokens[*pointer].type != OP && tokens[*pointer].value != BR)
    {
        ERROR(SYN_ERROR)
        return nullptr;
    }
    (*pointer)++;

    Node* left = get_expression(tokens, pointer, error);

    if (tokens[*pointer].type != OP && tokens[*pointer].value != BR) // надо вложить проверку в макрос
    {
        ERROR(SYN_ERROR)
        return nullptr;
    }
    (*pointer)++;

    Node* right = get_operation(tokens, pointer, error);

    Node* val = make_node(OP, IF, left, right, error);

    return val;
}

Node* get_equality(Token *const tokens, size_t *const pointer, Err_param *const error)
{
    assert(tokens);
    assert(pointer);
    assert(error);

    Node* left = get_id(tokens, pointer, error);

    if (tokens[*pointer].type != OP && tokens[*pointer].value != EQUAL)
    {
        ERROR(SYN_ERROR)
        return nullptr;
    }
    (*pointer)++;

    Node* right = get_expression(tokens, pointer, error);

    Node* val = make_node(OP, EQUAL, left, right, error);

    return val;
}

bool op_match(Token token, const OperationParameters *const arr, size_t arr_size)
{
    assert(arr);

    for (size_t ind = 0; ind < arr_size; ind++)
    {
        if (token.type == OP && token.value == arr[ind].num)
            return true;
    }

    return false;
}

Node* get_expression(Token *const tokens, size_t *const pointer, Err_param *const error)
{
    assert(tokens);
    assert(pointer);
    assert(error);

    Node* val = get_t(tokens, pointer, error);

    bool need_here = op_match(tokens[*pointer], first_pr, FIRST_PR_AMT);

    while (need_here)
    {
        size_t op = tokens[*pointer].value;
        (*pointer)++;

        Node* val2 = get_t(tokens, pointer, error);
        val = make_node(OP, op, val, val2, error);
        need_here = op_match(tokens[*pointer], first_pr, FIRST_PR_AMT);
    }

    return val;
}

Node* get_t (Token *const tokens, size_t *const pointer, Err_param *const error)
{
    Node* val = get_brace(tokens, pointer, error);

    bool need_here = op_match(tokens[*pointer], sec_pr, SEC_PR_AMT);

    while (need_here)
    {
        size_t op = tokens[*pointer].value;
        (*pointer)++;

        Node* val2 = get_brace(tokens, pointer, error);
        val = make_node(OP, op, val, val2, error);
        need_here = op_match(tokens[*pointer], first_pr, FIRST_PR_AMT);
    }

    return val;
}

Node* get_brace (Token *const tokens, size_t* pointer, Err_param *const error)
{
    assert(tokens);
    assert(pointer);
    assert(error);
    
    bool is_var = false;

    Node* val = nullptr;

    //printf("type %d\n", tokens[*pointer].type);

    if (tokens[*pointer].type == OP)
    {
        if (tokens[*pointer].value == BR)
        {
            (*pointer)++;

            val = get_expression(tokens, pointer, error);

            //printf("pointer %d, type %d\n", *pointer, tokens[*pointer].type);

            if (tokens[*pointer].type != OP && tokens[*pointer].value != BR)
            {
                ERROR(SYN_ERROR)
                return nullptr;
            }

            (*pointer)++;
        }
        else
        {
            //printf("UNARY\n");
            val = get_unary(tokens, pointer, error);
        }
        //else ??   
    }
    else if (tokens[*pointer].type == NUM)
        val = get_num(tokens, pointer, error);
    else 
    {
        //printf("GET ID\n");
        val = get_id(tokens, pointer, error);
    }
        
    return val;
}

Node* get_unary(Token *const tokens, size_t *const pointer, Err_param *const error)
{
    assert(pointer);
    assert(error);

    size_t op = tokens[*pointer].value;
    (*pointer)++;

    Node* val = get_expression(tokens, pointer, error);

    val = make_node(OP, op, nullptr, val, error);

    return val;
}

Node* get_num(Token *const token, size_t *const pointer, Err_param *const error)
{
    assert(pointer);
    assert(error);

    double value = token[*pointer].value;
    (*pointer)++;

    Node* val = make_node(NUM, value, nullptr, nullptr, error);

    return val;
}

Node* get_id(Token *const token, size_t *const pointer, Err_param *const error)
{
    assert(pointer);
    assert(error);

    double id_num = token[*pointer].value;
    (*pointer)++;

    Node* val = make_node(ID, id_num, nullptr, nullptr, error);

    return val;
}