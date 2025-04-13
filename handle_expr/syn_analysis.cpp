#include <stdio.h>

#include "syn_analysis.h"

static Node* syn_analysis(Token *const tokens, Id *const ids, ErrList *const list);

static Node* get_together(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);

static Node* get_operation(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_binary_op(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_unary_op(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* op_case(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_else(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_printf(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_func(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_func_def (Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);

static Node* get_act(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_part(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_expr(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);

static Node* get_sec_pr(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_first_pr(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);

static Node* get_brace(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
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

    if (tokens[pointer].type != OP && tokens[pointer].value != END && pointer >= TOKEN_AMT)
    {
        printf("you didnt add end mark!\n");
        ERROR(SYN_ERROR)
        return nullptr;
    }

    pointer++;

    return root;
}

Node* get_together(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT
    
    Node* val = nullptr;

    bool is_end = false;
    
    if (tokens[*pointer].type == OP && tokens[*pointer].value == END)
        is_end = true;

    val = get_operation(tokens, ids, pointer, list);
    RETURN_PTR

    while (!is_end && *pointer <= TOKEN_AMT)
    {
        if (tokens[*pointer].type == OP && tokens[*pointer].value == SEP)
        {
            (*pointer)++;

            Node* val2 = get_operation(tokens, ids, pointer, list);
            RETURN_PTR

            val = make_node(OP, SEP, val, val2, list);
            RETURN_PTR

            //graph_dump(val, ids, val, list);
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
    if (tokens[*pointer].type == OP && (tokens[*pointer].value == SEP || tokens[*pointer].value == END))
        is_end = true;

    while (!is_end && *pointer <= TOKEN_AMT)
    {
        if (tokens[*pointer].type == OP)
            val = op_case(tokens, ids, pointer, list);
            
        else if (tokens[*pointer].type == ID)
        {
            if (ids[tokens[*pointer].value].type == VAR_ID)
                val = get_expr(tokens, ids, pointer, list);
            else 
                val = get_func(tokens, ids, pointer, list);
        }
        RETURN_PTR

        if (tokens[*pointer].type == OP && (tokens[*pointer].value == SEP || tokens[*pointer].value == END))
            is_end = true;
    }

    //graph_dump(val, ids, val, list);

    return val;
}

Node* op_case(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* val = nullptr;

    (*pointer)++;

    bool is_binary = false;
    for (size_t op = 0; op < BINARY_OP_AMT; op++)
    {
        if (tokens[(*pointer) - 1].value == binary_op[op].num)
        {
            is_binary = true;
            break;
        }
    }

    bool is_unary = false;
    for (size_t op = 0; op < UNARY_OP_AMT; op++)
    {
        if (tokens[(*pointer) - 1].value == unary_op[op].num)
        {
            is_unary = true;
            break;
        }
    }

    if (is_binary)
        val = get_binary_op(tokens, ids, pointer, list);
    else if (is_unary)
        val = get_unary_op(tokens, ids, pointer, list);
    
    else if (tokens[(*pointer) - 1].value == ELSE)
        val = get_else(tokens, ids, pointer, list);
    else 
    {
        ERROR(SYN_ERROR)
        return nullptr;
    }

    //graph_dump(val, ids, val, list);
    //printf("TYPE %d, VALUE %d\n", tokens[*pointer].type, tokens[*pointer].value);

    return val;
}

Node* get_binary_op(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    size_t current_op = tokens[(*pointer) - 1].value;
           
    BR_CHECK
    Node* left = nullptr;
    if (current_op == FOR)
    {
        if (tokens[*pointer].type == NUM)
            left = get_num(tokens, ids, pointer, list);
        else 
            left = get_id(tokens, ids, pointer, list);
    }
    else
        left = get_expr(tokens, ids, pointer, list);
    BR_CHECK

    Node* right = get_act(tokens, ids, pointer, list);
    RETURN_PTR

    Node* val = make_node(OP, current_op, left, right, list);
    RETURN_PTR

    //graph_dump(val, ids, val, list);

    return val;
}

Node* get_unary_op(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    size_t current_op = tokens[(*pointer) - 1].value;

    Node* id = get_id(tokens, ids, pointer, list);
    RETURN_PTR

    Node* val = make_node(OP, current_op, id, nullptr, list);
    RETURN_PTR

    return val;
}

Node* get_act(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* right = nullptr;
    
    if (tokens[*pointer].type == OP && tokens[*pointer].value == BR)
    {
        (*pointer)++;

        right = get_part(tokens, ids, pointer, list);
        BR_CHECK
    }
    else
    {
        if (tokens[*pointer].type == OP)
        {
            if (tokens[*pointer].value == PRINTF)
            {
                (*pointer)++;
                right = get_printf(tokens, ids, pointer, list);
            }
            else
            {
                printf("you cant start here with this operation\n");
                ERROR(SYN_ERROR)
                return nullptr;
            }
        }
        else if (tokens[*pointer].type == ID)
        {
            if (ids[tokens[*pointer].value].type == VAR_ID)
            {
                right = get_expr(tokens, ids, pointer, list);
            }
            else 
            {
                right = make_node(ID, tokens[(*pointer) - 1].value, nullptr, nullptr, list);
            }
        }
    }
    RETURN_PTR

    return right;
}

Node* get_part(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT
    
    Node* val = nullptr;

    bool is_end = false;

    val = get_operation(tokens, ids, pointer, list);
    RETURN_PTR

    (*pointer)++;
    
    if (tokens[*pointer].type == OP && tokens[*pointer].value == BR)
        is_end = true;
    else
        (*pointer)--;

    while (!is_end && *pointer <= TOKEN_AMT)
    {
        if (tokens[*pointer].type == OP && tokens[*pointer].value == SEP)
        {
            (*pointer)++;

            Node* val2 = get_operation(tokens, ids, pointer, list);
            RETURN_PTR

            val = make_node(OP, SEP, val, val2, list);
            RETURN_PTR
        }

        (*pointer)++;
        
        if (tokens[(*pointer)].type == OP && tokens[(*pointer)].value == BR)
            is_end = true;
        
        else
            (*pointer)--;
        
    }

    return val;
}

Node* get_else(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* act = get_act(tokens, ids, pointer, list);
    RETURN_PTR

    Node* val = make_node(OP, ELSE, nullptr, act, list);
    RETURN_PTR

    return val;
}


Node* get_printf(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* id = get_id(tokens, ids, pointer, list);
    RETURN_PTR

    Node* val = make_node(OP, PRINTF, id, nullptr, list);
    RETURN_PTR

    return val;
}

Node* get_expr(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* left = get_id(tokens, ids, pointer, list);
    RETURN_PTR

    bool is_expr_op = false;

    if (tokens[*pointer].type != OP)
    {
        printf("you cant use this in expression, there should be an operation\n");
        ERROR(SYN_ERROR)
        return nullptr;
    }

    for (size_t op = 0; op < EXPR_OP_AMT; op++)
    {
        if (tokens[*pointer].value == expr_op[op].num)
        {
            //printf("USE OPERATION %d\n", expr_op[op].num);
            is_expr_op = true;
        }
    }
    
    if (!is_expr_op)
    {
        printf("you cant use this operation in expression\n");
        ERROR(SYN_ERROR)
        return nullptr;
    }

    int operation = tokens[*pointer].value;
    (*pointer)++;

    Node* right = nullptr;

    if (tokens[*pointer].type == OP)
    {
        if (tokens[*pointer].value == SQRT)
        {
            (*pointer)++;
            Node* arg = get_id(tokens, ids, pointer, list);

            right = make_node(OP, SQRT, arg, nullptr, list);
        }
        else if (tokens[*pointer].value == CALC_BR)
        {
            right = get_sec_pr(tokens, ids, pointer, list);
        }
        else
        {
            printf("you cant use it here\n");
            ERROR(SYN_ERROR)
        }
    }
    else
    {
        right = get_sec_pr(tokens, ids, pointer, list);
    }
    RETURN_PTR

    Node* val = make_node(OP, operation, left, right, list);
    //graph_dump(val, ids, val, list);

    return val;
}


Node* get_id(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* val = make_node(ID, tokens[*pointer].value, nullptr, nullptr, list);
    RETURN_PTR

    (*pointer)++;

    return val;
}

Node* get_sec_pr(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT
    
    Node* val = nullptr;

    val = get_first_pr(tokens, ids, pointer, list);
    RETURN_PTR

    bool is_sec_pr = false;
    for (size_t op = 0; op < SEC_PR_AMT; op++)
    {
        if (tokens[*pointer].value == sec_pr[op].num && tokens[*pointer].type == OP)
            is_sec_pr = true;
    }
    while (is_sec_pr)
    {
        size_t the_op = tokens[*pointer].value;
        (*pointer)++;
        Node* val2 = get_first_pr(tokens, ids, pointer, list);
        RETURN_PTR
        val = make_node(OP, the_op, val, val2, list);
        RETURN_PTR

        is_sec_pr = false;
        for (size_t op = 0; op < SEC_PR_AMT; op++)
        {
            if (tokens[*pointer].value == sec_pr[op].num && tokens[*pointer].type == OP)
                is_sec_pr = true;
        }
    }

    return val;
}

Node* get_first_pr(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT
    
    Node* val = nullptr;

    bool is_end = false;

    val = get_brace(tokens, ids, pointer, list);
    RETURN_PTR

    bool is_first_pr = false;

    for (size_t op = 0; op < FIRST_PR_AMT; op++)
    {
        if (tokens[*pointer].value == first_pr[op].num && tokens[*pointer].type == OP)
            is_first_pr = true;
    }
    while (is_first_pr)
    {
        size_t the_op = tokens[*pointer].value;
        (*pointer)++;
        Node* val2 = get_brace(tokens, ids, pointer, list);
        RETURN_PTR
        val = make_node(OP, the_op, val, val2, list);
        RETURN_PTR

        is_first_pr = false;

        for (size_t op = 0; op < FIRST_PR_AMT; op++)
        {
            if (tokens[*pointer].value == first_pr[op].num && tokens[*pointer].type == OP)
                is_first_pr = true;
        }
    }

    return val;
}

Node* get_brace(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT
    
    Node* val = nullptr;

    if (tokens[*pointer].type == OP && tokens[*pointer].value == CALC_BR)
    {
        (*pointer)++;

        val = get_sec_pr(tokens, ids, pointer, list);
        RETURN_PTR

        CALC_BR_CHECK
    }
    else if (tokens[*pointer].type == NUM)
    {
        val = get_num(tokens, ids, pointer, list);
        RETURN_PTR
    }
    else if (tokens[*pointer].type == ID && ids[tokens[*pointer].value].type == VAR_ID)
    {
        val = get_id(tokens, ids, pointer, list);
        RETURN_PTR
    }
    else
    {
        printf("you cant use it in brace expressions\n");
        ERROR(SYN_ERROR)
        return nullptr;
    }

    return val;
}

Node* get_num(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    int value = tokens[*pointer].value;
    (*pointer)++;

    Node* val = make_node(NUM, value, nullptr, nullptr, list);
    RETURN_PTR

    return val;
}

Node* get_func(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* val = nullptr;

    if (tokens[(*pointer) + 1].type == OP)
    {
        if (tokens[(*pointer) + 1].value == SEP)
            val = get_id(tokens, ids, pointer, list);

        else if (tokens[(*pointer) + 1].value == BR)
            val = get_func_def(tokens, ids, pointer, list);
        
        else
        {
            printf("you cant use this operation here\n");
            ERROR(SYN_ERROR)
            return nullptr;
        }
        RETURN_PTR
    }
    else
    {
        printf("you cant use this here\n");
        ERROR(SYN_ERROR)
        return nullptr;
    }

    return val;
}

Node* get_func_def (Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* val = nullptr;

    size_t the_id = tokens[*pointer].value;
    (*pointer)++;

    Node* def = get_act(tokens, ids, pointer, list);
    RETURN_PTR

    val = make_node(ID, the_id, def, nullptr, list);

    return val;
}