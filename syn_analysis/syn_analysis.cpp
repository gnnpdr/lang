#include <stdio.h>

#include "syn_analysis.h"

static bool op_match(Token token, const OperationParameters *const arr, size_t arr_size);

static Node* get_operation(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error);
static Node* get_if(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error);
static Node* get_equality(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error);

static Node* get_together(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error);
static Node* get_expression(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error);
static Node* get_t (Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error);
static Node* get_brace (Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error);
static Node* get_unary(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error);

static Node* get_num(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error);
static Node* get_id(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error);

Node* syn_analysis(Token *const tokens, Id *const ids, Err_param *const error)
{
    assert(tokens);
    assert(error);

    size_t pointer = 0;

    Node* root = get_together(tokens, ids, &pointer, error);

    if (tokens[pointer].type != OP && tokens[pointer].value != END)
    {
        printf("ERROR\n");
        ERROR(SYN_ERROR)
        return nullptr;
    }

    pointer++;

    graph_dump(root, ids, root, error);

    return root;
}

Node* get_together(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error)
{
    Node* val = get_operation(tokens, ids, pointer, error);
    
    bool is_end = false;
    if (tokens[*pointer].type == OP && tokens[*pointer].value == END)
        is_end = true;

    while (!is_end)
    {
        //printf("GET TOGETHER\n");
        if (tokens[*pointer].type == OP && tokens[*pointer].value == SEP)
        {
            (*pointer)++;
            Node* val2 = get_operation(tokens, ids, pointer, error);

            val = make_node(OP, SEP, val, val2, error);
        }

        if (tokens[*pointer].type == OP && tokens[*pointer].value == END)
            is_end = true;
    }

    return val;
}

Node* get_operation(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error)
{
    assert(tokens);
    assert(pointer);
    assert(error);

    Node* val = nullptr;
    
    bool is_end = false;
    if (tokens[*pointer].type == OP && tokens[*pointer].value == SEP || tokens[*pointer].value == END)
        is_end = true;

    while (!is_end)
    {
        //сепараторная точка может пройти только если она в начале
        //printf("!! type %d, value %d\n", tokens[*pointer].type, tokens[*pointer].value);  //второй раз жолжен вывести разделительную точку
        //а в третий - айди 1 (второй короче)
        if (tokens[*pointer].type == OP)
        {
            if (tokens[*pointer].value == BR)
            {
                //printf("GET BRACE\n");
                (*pointer)++;

                size_t start_pointer = *pointer;

                val = get_operation(tokens, ids, pointer, error);  //здесь могут возникнуть проблемы?

                if (tokens[*pointer].type != OP && tokens[*pointer].value != BR)
                {
                    ERROR(SYN_ERROR)
                    return nullptr;
                }
                (*pointer)++;
            }
            else if (tokens[*pointer].value == IF)
            {
                //printf("GET IF\n");
                (*pointer)++;
                val = get_if(tokens, ids, pointer, error);
            }
        }
        else if (tokens[*pointer].type == ID)
        {
            ///printf("GET A\n");
            val = get_equality(tokens, ids, pointer, error);
        }

        /*if (tokens[*pointer].value == SEP)
        {
            printf("SEP\n");
            (*pointer)++;
            Node* val2 = get_operation(tokens, ids, pointer, error);
            val = make_node(OP, SEP, val, val2, error);
            graph_dump(val, ids, val, error);
        }*/

        if (tokens[*pointer].type == OP && tokens[*pointer].value == SEP || tokens[*pointer].value == END)
            is_end = true;
    }

    return val;
}

Node* get_if(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error)
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

    Node* left = get_expression(tokens, ids, pointer, error);
    //graph_dump(left, ids, left, error);

    if (tokens[*pointer].type != OP && tokens[*pointer].value != BR) // надо вложить проверку в макрос
    {
        ERROR(SYN_ERROR)
        return nullptr;
    }
    (*pointer)++;

    Node* right = get_operation(tokens, ids, pointer, error);  //после этого он не остановился на разделительном знаке, а пошел дальше и тут все просто по пизде
    //graph_dump(right, ids, right, error);

    Node* val = make_node(OP, IF, left, right, error);
    //printf("val %p, left %p, right %p\n", val, left, right);
    //graph_dump(val, ids, val, error);

    return val;
}

Node* get_equality(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error)
{
    assert(tokens);
    assert(pointer);
    assert(error);

    Node* left = get_id(tokens, ids, pointer, error);

    if (tokens[*pointer].type != OP && tokens[*pointer].value != EQUAL)
    {
        ERROR(SYN_ERROR)
        return nullptr;
    }
    (*pointer)++;

    Node* right = get_expression(tokens, ids, pointer, error);

    Node* val = make_node(OP, EQUAL, left, right, error);

    //graph_dump(val, ids, val, error);

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

Node* get_expression(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error)
{
    assert(tokens);
    assert(pointer);
    assert(error);

    Node* val = get_t(tokens, ids, pointer, error);

    bool need_here = op_match(tokens[*pointer], first_pr, FIRST_PR_AMT);

    while (need_here)
    {
        size_t op = tokens[*pointer].value;
        (*pointer)++;

        Node* val2 = get_t(tokens, ids, pointer, error);
        val = make_node(OP, op, val, val2, error);
        need_here = op_match(tokens[*pointer], first_pr, FIRST_PR_AMT);
    }

    return val;
}

Node* get_t (Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error)
{
    Node* val = get_brace(tokens, ids, pointer, error);

    bool need_here = op_match(tokens[*pointer], sec_pr, SEC_PR_AMT);

    while (need_here)
    {
        size_t op = tokens[*pointer].value;
        (*pointer)++;

        Node* val2 = get_brace(tokens, ids, pointer, error);
        val = make_node(OP, op, val, val2, error);
        need_here = op_match(tokens[*pointer], first_pr, FIRST_PR_AMT);
    }

    return val;
}

Node* get_brace (Token *const tokens, Id *const ids, size_t* pointer, Err_param *const error)
{
    assert(tokens);
    assert(pointer);
    assert(error);
    
    bool is_var = false;

    Node* val = nullptr;

    if (tokens[*pointer].type == OP)
    {
        if (tokens[*pointer].value == BR)
        {
            (*pointer)++;

            val = get_expression(tokens, ids, pointer, error);

            if (tokens[*pointer].type != OP && tokens[*pointer].value != BR)
            {
                ERROR(SYN_ERROR)
                return nullptr;
            }

            (*pointer)++;
        }
        else
        {
            //printf("GET UNARY\n");
            val = get_unary(tokens, ids, pointer, error);
        }
    }
    else if (tokens[*pointer].type == NUM)
    {
        //printf("GET NUM\n");
        val = get_num(tokens, ids, pointer, error);
    }
        
    else 
    {
        //printf("GET ID\n");
        val = get_id(tokens, ids, pointer, error);
    }
        
    return val;
}

Node* get_unary(Token *const tokens, Id *const ids, size_t *const pointer, Err_param *const error)
{
    assert(pointer);
    assert(error);

    size_t op = tokens[*pointer].value;
    (*pointer)++;

    Node* val = get_expression(tokens, ids, pointer, error);

    val = make_node(OP, op, nullptr, val, error);

    return val;
}

Node* get_num(Token *const token, Id *const ids, size_t *const pointer, Err_param *const error)
{
    assert(pointer);
    assert(error);

    double value = token[*pointer].value;
    (*pointer)++;

    Node* val = make_node(NUM, value, nullptr, nullptr, error);

    return val;
}

Node* get_id(Token *const token, Id *const ids, size_t *const pointer, Err_param *const error)
{
    assert(pointer);
    assert(error);

    double id_num = token[*pointer].value;
    (*pointer)++;

    Node* val = make_node(ID, id_num, nullptr, nullptr, error);

    return val;
}