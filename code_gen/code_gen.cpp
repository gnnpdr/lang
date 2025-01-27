#include <stdio.h>

#include "code_gen.h"

static void get_part(Node *const node, size_t *const if_cnt, size_t *const start_if_cnt, Id *const ids, char* asm_code, ErrList *const list);

static void code_if(Node* node, size_t *const if_cnt, size_t *const start_if_cnt, Id *const ids, char *const asm_code, ErrList *const list);
static void code_expr(Node* node, size_t *const if_cnt, Id *const ids, char *const asm_code, ErrList *const list);
static void code_expr_part(Node *const node, Id *const ids, char *const asm_code, ErrList *const list);


static void code_equal(Node* node, char *const asm_code, Id *const ids, ErrList *const list);
static void code_op(Node* node, Id *const ids, char *const asm_code, ErrList *const list);
static bool arithm_check(Node* node);

static void code_printf (Node* node, char *const asm_code, ErrList *const list);

static void code_cycle(Node* node, size_t *const if_cnt, size_t *const start_if_cnt, Id *const ids, char *const asm_code, ErrList *const list);

static void code_id_op(Node* node, size_t *const if_cnt, size_t *const start_if_cnt, Id *const ids, char *const asm_code, ErrList *const list);

static void node_push(Node* node, char *const str, ErrList *const list);
static void node_pop(Node* node, char *const str, ErrList *const list);

//надо вложить все группы ассертов в макросы

void code_gen(Tree *const the_tree, Id *const ids, ErrList *const list)
{
    assert(the_tree);
    assert(ids);
    assert(list);

    char* asm_code = (char*)calloc(MAX_FILE_SIZE, sizeof(char));
    ALLOCATION_CHECK_VOID(asm_code)

    size_t if_cnt = 0;
    size_t start_if_cnt = 0;

    Node* root = the_tree->root;
    get_part(root, &if_cnt, &start_if_cnt, ids, asm_code, list);
    RETURN_VOID

    int sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s%s\n", asm_code, HLT_STR);
    SPRINTF_CHECK_VOID

    fill_input_file(ASM_NAME, asm_code, list);  //добавить возможность менять название
    free(asm_code);
    
}

void get_part(Node *const node, size_t *const if_cnt, size_t *const start_if_cnt, Id *const ids, char* asm_code, ErrList *const list)
{
    assert(node);
    assert(asm_code);
    assert(list);
    assert(if_cnt);
    assert(start_if_cnt);

    int sprintf_res = 0;

    if (IS_SEP)
    {
        get_part(node->Left, if_cnt, start_if_cnt, ids, asm_code, list);
        RETURN_VOID

        sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s\n", asm_code);
        SPRINTF_CHECK_VOID

        get_part(node->Right, if_cnt, start_if_cnt, ids, asm_code, list);
        RETURN_VOID

        sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s\n", asm_code);
        SPRINTF_CHECK_VOID
    }

//------------if and similar--------------

    bool was_if = false;
    bool is_else_if = false;

    /*
        закодировать метку начала прыжк
        достижение этого фрагмета кода будет означать, что одно из условий прошло и надо прыгать на коне
        закрыть начальный иф (элс иф)
        закрывающая цель будет одна и та же, так что кодировать этот джамп можно одной меткой, 
        относящейся к этому конкретному иф
        номер стартового ифа (а именно он будет влиять на то, с каким номером будет сбрасывающая метка)
        будет пополняться, если достигнут стандартный иф, а не элз или элз и
        закодировать условие элс ифа отдельным ифом
        повторить с метками и закрытиями то, что было с иформ
        когда будет достигнут элс или последний элс иф, поставить метку конца ифа с элзами
    */

    if ((node->Right->type == OP && (node->Right->value == ELSE_IF || node->Right->value == IF || node->Right->value == ELSE)) ||
        (node->Left->Left->type == OP && (node->Left->Left->value == ELSE_IF || node->Left->Left->value == IF || node->Left->Left->value == ELSE)) ||
        (node->Left->Right->type == OP && (node->Left->Right->value == ELSE_IF || node->Left->Right->value == IF || node->Left->Right->value == ELSE)))
        was_if = true;

    size_t init_if_cnt = *if_cnt;
    bool alt_cnt = false;

    if (IS_IF)  
    {
        if (was_if)
        {
            alt_cnt = true;
            *if_cnt *= 10; //вводдится альтернативный счетчит
        }

        code_if(node, if_cnt, start_if_cnt, ids, asm_code, list); 

        if (alt_cnt)
            *if_cnt = init_if_cnt;  //считчик будет восстановлен после ретерна из функции
    }

    if (IS_ELSE_IF || IS_ELSE)
        is_else_if = true;

    if (was_if && !is_else_if)
    {
        //концевой
        sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%sIF_RES%d:\n", asm_code, *start_if_cnt);
        SPRINTF_CHECK_VOID
        (*start_if_cnt)++;
    }

    //после этого должна просто закодироваться следующая операция - следующая часть кода.
    //в любом случае эта метка должна быть поставлена, может, смещение с сепарирующего узла не дает посмотреть 

    if (is_else_if)
    {
        //начальный
        //условие для финального прыжка
        sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s\n%sIF_RES%d:\n", asm_code, JMP_STR, *start_if_cnt);
        SPRINTF_CHECK_VOID

        //закрытие условия
        sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s\nIF_COND%d:\n", asm_code, *if_cnt);
        SPRINTF_CHECK_VOID
        (*if_cnt)++; 

        if (IS_ELSE_IF)
            code_if(node, if_cnt, start_if_cnt, ids, asm_code, list); 
        
        else if (IS_ELSE)
            get_part(node->Right, if_cnt, start_if_cnt, ids, asm_code, list);
    
        RETURN_VOID
    }

//-------equal-------------

    if (IS_EQUAL)
        code_equal(node, asm_code, ids, list);  

//-----cycles----

    if (IS_WHILE || IS_FOR)
    {
        if (was_if)
        {
            alt_cnt = true;
            *if_cnt *= 10; //вводдится альтернативный счетчит
        }

        code_cycle(node, if_cnt, start_if_cnt, ids, asm_code, list); 

        if (alt_cnt)
            *if_cnt = init_if_cnt;
    }

//----------other---------
    
    if (IS_PRINTF)
        code_printf (node, asm_code, list);

    if (node->type == ID)
        code_id_op(node, if_cnt, start_if_cnt, ids, asm_code, list);
    
    return;
}

//--------------------IF---------------------

void code_if(Node* node, size_t *const if_cnt, size_t *const start_if_cnt, Id *const ids, char *const asm_code, ErrList *const list)
{
    assert(node);
    assert(asm_code);
    assert(list);
    assert(if_cnt);
    assert(start_if_cnt);
    assert(ids);

    code_expr(node->Left, if_cnt, ids, asm_code, list);
    RETURN_VOID
    
    (*if_cnt)++;

    get_part(node->Right, if_cnt, start_if_cnt, ids, asm_code, list);  //предполагается возможность дальнейшего разветвления
    RETURN_VOID
}

void code_expr(Node* node, size_t *const if_cnt, Id *const ids, char *const asm_code, ErrList *const list)
{
    assert(node);
    assert(asm_code);
    assert(list);
    assert(if_cnt);
    assert(ids);

    int sprintf_res = 0;

    bool less = false;
    if (IS_LESS || IS_GREATER)
    {
        if (IS_LESS)
            less = true;

        code_expr_part(node->Right, ids, asm_code, list);
        code_expr_part(node->Left, ids, asm_code, list);

        if (less)
            sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s\n%s IF_COND%d:\n", asm_code, JA_STR, *if_cnt);
        else
            sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s\n%s IF_COND%d:\n", asm_code, JB_STR, *if_cnt);
    }
    else 
    {
        code_expr_part(node, ids, asm_code, list);
        int sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s%s 0\n%s IF_COND%d:\n", asm_code, PUSH_STR, JNE_STR, *if_cnt); 
    }
    SPRINTF_CHECK_VOID
}

void code_expr_part(Node *const node, Id *const ids, char *const asm_code, ErrList *const list)
{
    assert(node);
    assert(asm_code);
    assert(ids);
    assert(list);

    if (node->type == NUM || node->type == ID)
        node_push(node, asm_code, list);
    else
        code_op(node, ids, asm_code, list);
    RETURN_VOID
}

//------------EQUAL------------

void code_equal(Node* node, char *const asm_code, Id *const ids, ErrList *const list)
{
    assert(node);
    assert(asm_code);
    assert(ids);
    assert(list);

    code_expr_part(node->Right, ids, asm_code, list);
    RETURN_VOID

    node_pop(node->Left, asm_code, list);
    RETURN_VOID
}

//----------------ARITHM OPERATIONS----------------

void code_op(Node* node, Id *const ids, char *const asm_code, ErrList *const list)
{
    assert(node);
    assert(ids);
    assert(asm_code);
    assert(list);

    bool is_left_arithm = arithm_check(node->Left);
     
    if (is_left_arithm)
        code_op(node->Left, ids, asm_code, list);
    
    else
        node_push(node->Left, asm_code, list);

    RETURN_VOID

    node_push(node->Right, asm_code, list);
    RETURN_VOID
    
    for (int i = 0; i < ARITHM_CMD_AMT; i++)
    {
        if (node->type == OP && ArithmCmds[i].num == node->value)
        {
            int sprintf_res = sprintf_s(asm_code, MAX_STR_LEN, "%s%s\n", asm_code, appropriate_cmds[i].cmd.cmd_str);
            SPRINTF_CHECK_VOID
            break;
        }
    }
}

bool arithm_check(Node* node)
{
    assert(node);

    bool is_arithm = false;
    
    for (size_t cmd = 0; cmd < ARITHM_CMD_AMT; cmd++)
    {
        if (node->type == OP && node->value == ArithmCmds[cmd].num);
        {
            is_arithm = true;
        }
    }

    return is_arithm;
}

//----------PRINTF--------------

void code_printf (Node* node, char *const asm_code, ErrList *const list)
{
    assert(node);
    assert(asm_code);
    assert(list);

    node_push(node->Left, asm_code, list);
    RETURN_VOID

    int sprintf_res = sprintf_s(asm_code, MAX_STR_LEN, "%s%s\n", asm_code, OutStr.cmd_str);
    SPRINTF_CHECK_VOID
}

//----------CYCLES------------------

void code_cycle(Node* node, size_t *const if_cnt, size_t *const start_if_cnt, Id *const ids, char *const asm_code, ErrList *const list)
{
    assert(node);
    assert(start_if_cnt);
    assert(if_cnt);
    assert(ids);
    assert(asm_code);
    assert(list);

    //next cnt может идти в ногу с if cnt
    int sprintf_res = sprintf_s(asm_code, MAX_STR_LEN, "%sNEXT%d:\n", asm_code, *if_cnt);
    SPRINTF_CHECK_VOID

    code_expr(node->Left, if_cnt, ids, asm_code, list);
    RETURN_VOID

    get_part(node->Right, if_cnt, start_if_cnt, ids, asm_code, list);
    RETURN_VOID

    sprintf_res = sprintf_s(asm_code, MAX_STR_LEN, "%s%s NEXT%d:\n", asm_code, JMP_STR, *if_cnt);
    SPRINTF_CHECK_VOID

    sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s\nIF_COND%d:\n", asm_code, *if_cnt);
    SPRINTF_CHECK_VOID
    (*if_cnt)++; 
}

//-----------ID OPERATIONS--------------

void code_id_op(Node* node, size_t *const if_cnt, size_t *const start_if_cnt, Id *const ids, char *const asm_code, ErrList *const list)
{
    assert(node);
    assert(start_if_cnt);
    assert(if_cnt);
    assert(ids);
    assert(asm_code);
    assert(list);

    int sprintf_res = 0;
    char* cmd_name = (char*)calloc(MAX_STR_LEN, sizeof(char));
    strncpy(cmd_name, ids[node->value].start_address, ids[node->value].len);
    CPY_CHECK(cmd_name);

    //USE
    if (node->Left == nullptr && node->Right == nullptr)
    {
        //ссылка на функцию, соответствующую данному айди 
        sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s\ncall %s\n", asm_code, cmd_name);
        //с переменными все в порядке, во время всего проигрывания программы по каждой функции будет вестись
        //счетчик, который и будет пояснять, сколько переменных всего и какую часть массива перемнных надо брать
    }
    //DEFINE
    else
    {
        sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s\n%s\n", asm_code, cmd_name);
        SPRINTF_CHECK_VOID

        get_part(node->Left, if_cnt, start_if_cnt, ids, asm_code, list);
        RETURN_VOID

        sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s\nret\n", asm_code);
    }
    SPRINTF_CHECK_VOID
}

//-----------BASE--------------

void node_push(Node* node, char *const str, ErrList *const list)
{
    assert(node);
    assert(str);
    assert(list);

    int sprintf_res = 0;
    if (node->type == NUM)
    {
        sprintf_res = sprintf_s(str, MAX_FILE_SIZE, "%s%s %d\n", str, PUSH_STR, node->value);
    }
    else if(node->type == ID)
        sprintf_res = sprintf_s(str, MAX_FILE_SIZE, "%s%s [%d]\n", str, PUSH_STR, node->value); 
    
    SPRINTF_CHECK_VOID
}

void node_pop(Node* node, char *const str, ErrList *const list)
{
    assert(node);
    assert(str);
    assert(list);

    int sprintf_res = 0;

    if (node->type == NUM)
    {
        sprintf_res = (str, MAX_FILE_SIZE, "%s%s %d\n", str, POP_STR, node->value);
    }
    else if(node->type == ID)
    {
        sprintf_res = sprintf_s(str, MAX_FILE_SIZE, "%s%s [%d]\n", str, POP_STR, node->value);
    }
    SPRINTF_CHECK_VOID
}
