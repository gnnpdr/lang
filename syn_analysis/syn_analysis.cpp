#include <stdio.h>

#include "syn_analysis.h"

static Node* syn_analysis(Token *const tokens, Id *const ids, ErrList *const list);

static Node* get_together(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);

static Node* get_operation(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
static Node* get_binary_op(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list);
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

Node* analyse_text(Token *const tokens, Id *const ids, Input *const base_text, ErrList *const list)
{
    assert(tokens);
    assert(ids);
    assert(list);
    assert(base_text);

    lex_analysis(tokens, ids, base_text, list);
	RETURN_PTR

	Node* root = syn_analysis(tokens, ids, list);
	RETURN_PTR

    return root;
}

//выводит итоговый узел
Node* syn_analysis(Token *const tokens, Id *const ids, ErrList *const list)
{
    assert(tokens);
    assert(ids);
    assert(list);

    size_t pointer = 0;

    Node* root = get_together(tokens, ids, &pointer, list);
    RETURN_PTR

    if (tokens[pointer].type != OP && tokens[pointer].value != END && pointer >= TOKEN_AMT)  //все токены исчерпаны, а конец не достигнут
    {
        printf("you didnt add end mark!\n");
        ERROR(SYN_ERROR)
        return nullptr;
    }

    pointer++;

    return root;
}

//объединяет выражения точками
//прерывается на end токене или тогда, когда все токены исчерпаны
Node* get_together(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT
    
    Node* val = nullptr;

    bool is_end = false;
    
    if (tokens[*pointer].type == OP && tokens[*pointer].value == END)
        is_end = true;

    printf("GET OP\n");
    val = get_operation(tokens, ids, pointer, list);
    RETURN_PTR

    while (!is_end && *pointer <= TOKEN_AMT)
    {
        
        if (tokens[*pointer].type == OP && tokens[*pointer].value == SEP)
        {
            //graph_dump(val, ids, val, list);
            printf("TRY TO MAKE TOGETHER\n");
            (*pointer)++;

            Node* val2 = get_operation(tokens, ids, pointer, list);
            RETURN_PTR
            //graph_dump(val2, ids, val2, list);

            val = make_node(OP, SEP, val, val2, list);
            RETURN_PTR

            //graph_dump(val, ids, val, list);
        }

        if (tokens[*pointer].type == OP && tokens[*pointer].value == END)
            is_end = true;

    }

    return val;
}

//берет операцию - равенство, while, for, if
//если достигнут end или сепаратор, то прерывается
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
        {
            printf("OP\n");
            (*pointer)++; // пропуск операции

            bool is_binary = false;
            for (size_t op = 0; op < BINARY_OP_AMT; op++)
            {
                if (tokens[(*pointer) - 1].value == binary_op[op].num)
                {
                    is_binary = true;
                }
            }

            if (is_binary)
            
            {
                printf("binary op\ntoken %d %d\n", tokens[(*pointer) - 1].type, tokens[(*pointer) - 1].value);

                val = get_binary_op(tokens, ids, pointer, list);
            }
            // ELSE
            else if (tokens[(*pointer) - 1].value == ELSE)
            {
                val = get_else(tokens, ids, pointer, list);
            }
            // PRINTF
            else if (tokens[(*pointer) - 1].value == PRINTF)
            {
                printf("PRINTF\n");
                val = get_printf(tokens, ids, pointer, list);
            }   
            else 
            {
                printf ("-2 %d %d\n-1 %d %d\n0 %d %d\n1 %d %d\n2 %d %d\n", tokens[(*pointer) - 2].type, tokens[(*pointer) - 2].value, tokens[(*pointer) - 1].type, tokens[(*pointer) - 1].value, tokens[(*pointer) - 0].type, tokens[(*pointer) - 0].value, tokens[(*pointer) + 1].type, tokens[(*pointer) + 1].value, tokens[(*pointer) + 2].type, tokens[(*pointer) + 2].value);
                printf("no operations starting with this operation\n");
                ERROR(SYN_ERROR)
                return nullptr;
            }
        }
        else if (tokens[*pointer].type == ID)
        {
            printf("ID\n");
            // EXPR
            if (ids[tokens[*pointer].value].type == VAR_ID)
            {
                printf("VAR ID\n");
                val = get_expr(tokens, ids, pointer, list);
            }
            else 
            {
                printf("FUNC ID\n");
                val = get_func(tokens, ids, pointer, list);
            }
        }
        RETURN_PTR

        printf("CURRENT TOKEN IN OPERATIONS %d %d\n", tokens[*pointer].type, tokens[*pointer].value);

        if (tokens[*pointer].type == OP && (tokens[*pointer].value == SEP || tokens[*pointer].value == END))
            is_end = true;
    }

    printf("RET OPERATIONS\n");
    return val;
}

Node* get_binary_op(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    size_t current_op = tokens[(*pointer) - 1].value;

    BR_CHECK
    Node* left = get_expr(tokens, ids, pointer, list);
    BR_CHECK

    //graph_dump(left, ids, left, list);

    printf("GET ACT\n");
    Node* right = get_act(tokens, ids, pointer, list);
    RETURN_PTR

    Node* val = make_node(OP, current_op, left, right, list);
    RETURN_PTR

    graph_dump(val, ids, val, list);

    return val;
}

Node* get_act(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* right = nullptr;

    if (tokens[*pointer].type == OP && tokens[*pointer].value == BR)
    {
        printf("BRACE ACT\n");
        (*pointer)++;

        right = get_part(tokens, ids, pointer, list);

        BR_CHECK
    }
    else
    {
        if (tokens[*pointer].type == OP)
        {
            if (tokens[*pointer].value == PRINTF)
                right = get_printf(tokens, ids, pointer, list);
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

//скобки в основных выражениях
Node* get_part(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT
    
    Node* val = nullptr;

    bool is_end = false;

    printf("GET FIRST OP\n");
    val = get_operation(tokens, ids, pointer, list);
        RETURN_PTR
    
    if (tokens[*pointer].type == OP && tokens[*pointer].value == BR)
        is_end = true;

    while (!is_end && *pointer <= TOKEN_AMT)
    {
        if (tokens[*pointer].type == OP && tokens[*pointer].value == SEP)
        {
            (*pointer)++;

            printf("GET OP\n");
            Node* val2 = get_operation(tokens, ids, pointer, list);    //!!!!!!!!!!!
            RETURN_PTR

            val = make_node(OP, SEP, val, val2, list);
            RETURN_PTR
        }

        if (tokens[(*pointer) + 1].type == OP && tokens[(*pointer) + 1].value == BR)
            is_end = true;

    }

    return val;
}

Node* get_else(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* act = get_act(tokens, ids, pointer, list);
    RETURN_PTR

    Node* val = make_node(OP, ELSE, nullptr, act, list);  //по аналогии с предыдущими - действие в правой части
    RETURN_PTR

    return val;
}


Node* get_printf(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* id = get_id(tokens, ids, pointer, list);
    RETURN_PTR

    Node* val = make_node(OP, PRINTF, id, nullptr, list);  //по аналогии с предыдущими - действие в правой части
    RETURN_PTR

    (*pointer)++;  //пошел дальше на сепаратор 

    return val;
}

Node* get_expr(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* left = get_id(tokens, ids, pointer, list);
    RETURN_PTR

    printf("GOT ID\n");

    (*pointer)++;

    bool is_expr_op = false;

    if (tokens[*pointer].type != OP)   //эту большую проверку можно вложить в макрос
    {
        printf("you cant use this in expression, there should be an operation\n");
        ERROR(SYN_ERROR)
        return nullptr;
    }

    for (size_t op = 0; op < EXPR_OP_AMT; op++)
    {
        if (tokens[*pointer].value == expr_op[op].num)
            is_expr_op = true;
    }
    
    if (!is_expr_op)
    {
        printf ("-2 %d %d\n-1 %d %d\n0 %d %d\n1 %d %d\n2 %d %d\n", tokens[(*pointer) - 2].type, tokens[(*pointer) - 2].value, tokens[(*pointer) - 1].type, tokens[(*pointer) - 1].value, tokens[(*pointer) - 0].type, tokens[(*pointer) - 0].value, tokens[(*pointer) + 1].type, tokens[(*pointer) + 1].value, tokens[(*pointer) + 2].type, tokens[(*pointer) + 2].value);
        printf("you cant use this operation in expression\n");
        ERROR(SYN_ERROR)
        return nullptr;
    }

    int operation = tokens[*pointer].value;
    (*pointer)++;  // пропустили операцию = > < 

    printf("GET SEC PR\n");
    Node* right = get_sec_pr(tokens, ids, pointer, list);
    RETURN_PTR

    printf("GOT SEC PR OP\n");

    Node* val = make_node(OP, operation, left, right, list);

    //graph_dump(val, ids, val, list);
    //printf("CURRENT TOKEN %d %d\n", tokens[*pointer].type, tokens[*pointer].value);
    return val;
}


Node* get_id(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT

    Node* val = make_node(ID, tokens[*pointer].value, nullptr, nullptr, list);
    RETURN_PTR

    return val;
}


Node* get_sec_pr(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT
    
    Node* val = nullptr;

    bool is_end = false;
    
    if (tokens[*pointer].type == OP && (tokens[*pointer].value == END || tokens[*pointer].value == SEP || tokens[*pointer].value == BR))
        is_end = true;

    while (!is_end && *pointer <= TOKEN_AMT)
    {
        printf("GET FIRST PR\n");
        val = get_first_pr(tokens, ids, pointer, list);
        RETURN_PTR

        bool is_sec_pr = false;

        for (size_t op = 0; op < SEC_PR_AMT; op++)
        {
            if (tokens[*pointer].value == sec_pr[op].num && tokens[*pointer].value == OP)
                is_sec_pr = true;
        }

        if (is_sec_pr)
        {
            size_t the_op = tokens[*pointer].value;
            (*pointer)++;

            Node* val2 = get_first_pr(tokens, ids, pointer, list);
            RETURN_PTR

            val = make_node(OP, the_op, val, val2, list);
            RETURN_PTR
        }

        if (tokens[*pointer].type == OP && (tokens[*pointer].value == END || tokens[*pointer].value == SEP || tokens[*pointer].value == BR))
            is_end = true;
    }

    return val;
}

Node* get_first_pr(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT
    
    Node* val = nullptr;

    bool is_end = false;
    
    if (tokens[*pointer].type == OP && (tokens[*pointer].value == END || tokens[*pointer].value == SEP || tokens[*pointer].value == BR))
        is_end = true;

    while (!is_end && *pointer <= TOKEN_AMT)
    {
        printf("GET BRACE\n");
        val = get_brace(tokens, ids, pointer, list);
        RETURN_PTR

        bool is_first_pr = false;

        for (size_t op = 0; op < FIRST_PR_AMT; op++)
        {
            if (tokens[*pointer].value == first_pr[op].num && tokens[*pointer].value == OP)
                is_first_pr = true;
        }

        if (is_first_pr)
        {
            size_t the_op = tokens[*pointer].value;
            (*pointer)++;

            Node* val2 = get_brace(tokens, ids, pointer, list);
            RETURN_PTR

            val = make_node(OP, the_op, val, val2, list);
            RETURN_PTR
        }

        if (tokens[*pointer].type == OP && (tokens[*pointer].value == END || tokens[*pointer].value == SEP || tokens[*pointer].value == BR))
            is_end = true;
    }

    return val;
}

Node* get_brace(Token *const tokens, Id *const ids, size_t *const pointer, ErrList *const list)
{
    REC_ASSERT
    
    Node* val = nullptr;

    if (tokens[*pointer].type == OP && tokens[*pointer].value == CALC_BR)
    {
        printf("REALLY BRACES\n");
        (*pointer)++;

        val = get_sec_pr(tokens, ids, pointer, list);
        RETURN_PTR

        CALC_BR_CHECK
    }
    else if (tokens[*pointer].type == NUM)
    {
        printf("GET NUM\n");
        val = get_num(tokens, ids, pointer, list);
        RETURN_PTR
    }
    else
    {
        printf("CURRENT TOKEN %d %d\n", tokens[*pointer].type, tokens[*pointer].value);

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
        // FUNC_USE
        if (tokens[(*pointer) + 1].value == SEP)
        {
            val = get_id(tokens, ids, pointer, list);
            (*pointer)++;
        }
        // FUC_DEF
        else if (tokens[(*pointer) + 1].value == BR)
        {
            val = get_func_def(tokens, ids, pointer, list);
        }
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

    (*pointer)++;  //пропуск скобки

    Node* def = get_part(tokens, ids, pointer, list);
    RETURN_PTR

    (*pointer)++;  //пропуск скобки

    val = make_node(ID, the_id, def, nullptr, list);
}