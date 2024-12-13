#include <stdio.h>

#include "syn_analysis.h"

static bool op_match(Token token, const OperationParameters *const arr, size_t arr_size);

static Node* get_operation(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_if(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_equality(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);

static Node* get_together(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_expression(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_t (Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_brace (Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_unary(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);

static Node* get_num(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_id(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);

Node* syn_analysis(Token *const tokens, Id *const ids, ErrList *const list)
{
    assert(tokens);
    assert(ids);
    assert(list);

    size_t pointer = 0;

    Node* root = get_together(tokens, ids, &pointer, list);
    RETURN_PTR

    if (tokens[pointer].type != OP && tokens[pointer].value != END)
    {
        printf("you didnt add end mark!\n");  //стоит так по всем ошибкам расписать
        ERROR(SYN_ERROR)
        return nullptr;
    }

    pointer++;

    graph_dump(root, ids, root, list);

    return root;
}

Node* get_together(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* val = get_operation(tokens, ids, pointer, list);
    RETURN_PTR
    
    bool is_end = false;
    if (tokens[*pointer].type == OP && tokens[*pointer].value == END)
        is_end = true;

    while (!is_end)
    {
        if (tokens[*pointer].type == OP && tokens[*pointer].value == SEP)
        {
            (*pointer)++;
            Node* val2 = get_operation(tokens, ids, pointer, list);
            RETURN_PTR

            val = make_node(OP, SEP, val, val2, list);
            RETURN_PTR
        }

        if (tokens[*pointer].type == OP && tokens[*pointer].value == END)
            is_end = true;
    }

    return val;
}

Node* get_operation(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* val = nullptr;
    
    bool is_end = false;
    if (tokens[*pointer].type == OP && tokens[*pointer].value == SEP || tokens[*pointer].value == END)
        is_end = true;

    while (!is_end)
    {
        if (tokens[*pointer].type == OP)
        {
            if (tokens[*pointer].value == BR)
            {
                (*pointer)++;

                size_t start_pointer = *pointer;

                val = get_operation(tokens, ids, pointer, list);
                RETURN_PTR

                if (tokens[*pointer].type != OP && tokens[*pointer].value != BR)
                {
                    printf("you dont have a brace at char number %d\n", *pointer); //надо строчки тоже считать
                    ERROR(SYN_ERROR)
                    return nullptr;
                }
                (*pointer)++;
            }
            else if (tokens[*pointer].value == IF)
            {
                (*pointer)++;
                val = get_if(tokens, ids, pointer, list);
                RETURN_PTR
            }
        }
        else if (tokens[*pointer].type == ID)
        {
            val = get_equality(tokens, ids, pointer, list);
            RETURN_PTR
        }

        if (tokens[*pointer].type == OP && tokens[*pointer].value == SEP || tokens[*pointer].value == END)
            is_end = true;
    }

    return val;
}

Node* get_if(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    BR_CHECK

    Node* left = get_expression(tokens, ids, pointer, list);
    RETURN_PTR

    BR_CHECK
    
    Node* right = get_operation(tokens, ids, pointer, list);
    RETURN_PTR

    Node* val = make_node(OP, IF, left, right, list);
    RETURN_PTR

    return val;
}

Node* get_equality(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* left = get_id(tokens, ids, pointer, list);
    RETURN_PTR

    if (tokens[*pointer].type != OP && tokens[*pointer].value != EQUAL)
    {
        printf("you dont have an equal mark ia equality...\n");
        ERROR(SYN_ERROR)
        return nullptr;
    }
    (*pointer)++;

    Node* right = get_expression(tokens, ids, pointer, list);
    RETURN_PTR

    Node* val = make_node(OP, EQUAL, left, right, list);
    RETURN_PTR

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

Node* get_expression(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* val = get_t(tokens, ids, pointer, list);
    RETURN_PTR

    bool need_here = op_match(tokens[*pointer], first_pr, FIRST_PR_AMT);

    while (need_here)
    {
        size_t op = tokens[*pointer].value;
        (*pointer)++;

        Node* val2 = get_t(tokens, ids, pointer, list);
        RETURN_PTR
        val = make_node(OP, op, val, val2, list);
        RETURN_PTR

        need_here = op_match(tokens[*pointer], first_pr, FIRST_PR_AMT);
    }

    return val;
}

Node* get_t (Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* val = get_brace(tokens, ids, pointer, list);
    RETURN_PTR

    bool need_here = op_match(tokens[*pointer], sec_pr, SEC_PR_AMT);

    while (need_here)
    {
        size_t op = tokens[*pointer].value;
        (*pointer)++;

        Node* val2 = get_brace(tokens, ids, pointer, list);
        RETURN_PTR
        val = make_node(OP, op, val, val2, list);
        RETURN_PTR
        need_here = op_match(tokens[*pointer], first_pr, FIRST_PR_AMT);
    }

    return val;
}

Node* get_brace (Token *const tokens, Id *const ids, size_t* pointer, ErrList *const list)
{
    REC_ASSERT
    
    bool is_var = false;

    Node* val = nullptr;

    if (tokens[*pointer].type == OP)
    {
        if (tokens[*pointer].value == BR)
        {
            (*pointer)++;

            val = get_expression(tokens, ids, pointer, list);
            RETURN_PTR

            BR_CHECK
        }
        else
            val = get_unary(tokens, ids, pointer, list);
    }
    else if (tokens[*pointer].type == NUM)
        val = get_num(tokens, ids, pointer, list);
    
    else 
        val = get_id(tokens, ids, pointer, list);

    RETURN_PTR  
    return val;
}

Node* get_unary(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    size_t op = tokens[*pointer].value;
    (*pointer)++;

    Node* val = get_expression(tokens, ids, pointer, list);
    RETURN_PTR

    val = make_node(OP, op, nullptr, val, list);
    RETURN_PTR

    return val;
}

Node* get_num(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    double value = tokens[*pointer].value;
    (*pointer)++;

    Node* val = make_node(NUM, value, nullptr, nullptr, list);
    RETURN_PTR

    return val;
}

Node* get_id(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    double id_num = tokens[*pointer].value;
    (*pointer)++;

    Node* val = make_node(ID, id_num, nullptr, nullptr, list);
    RETURN_PTR

    return val;
}