#include <stdio.h>

#include "code_gen.h"

static void get_part(Node *const node, size_t *const if_cnt, size_t *const start_if_cnt, Id *const ids, char* asm_code, ErrList *const list);

static void code_if(Node* node, Id *const ids, char *const str, ErrList *const list);
static void code_equal(Node* node, char *const str, ErrList *const list);
static void code_op(Node* node, Id *const ids, char *const str, ErrList *const list);

static void node_push(Node* node, char *const str, ErrList *const list);
static void node_pop(Node* node, char *const str, ErrList *const list);

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
    bool is_if = false;

    /*
            //закодировать метку начала прыжка

            //достижение этого фрагмета кода будет означать, что одно из условий прошло и надо прыгать на конец

            //закрыть начальный иф (элс иф)
            //закрывающая цель будет одна и та же, так что кодировать этот джамп можно одной меткой, 
            //относящейся к этому конкретному ифу

            //номер стартового ифа (а именно он будет влиять на то, с каким номером будет сбрасывающая метка)
            //будет пополняться, если достигнут стандартный иф, а не элз или элз иф

            //закодировать условие элс ифа отдельным ифом
            //повторить с метками и закрытиями то, что было с иформ
            //когда будет достигнут элс или последний элс иф, поставить метку конца ифа с элзами
    
        */

    if (IS_IF)  
    {
        code_if(node, ids, asm_code, list); 
    }

    if ((node->Right->type == OP && (node->Right->value == ELSE_IF || node->Right->value == IF || node->Right->value == ELSE)) ||
        (node->Left->Left->type == OP && (node->Left->Left->value == ELSE_IF || node->Left->Left->value == IF || node->Left->Left->value == ELSE)) ||
        (node->Left->Right->type == OP && (node->Left->Right->value == ELSE_IF || node->Left->Right->value == IF || node->Left->Right->value == ELSE)))
        was_if = true;

    if (IS_ELSE_IF || IS_ELSE)
        is_if = true;

    if (was_if && !is_if)
    {
        //концевой
        sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%sIF_RES%d:\n", asm_code, *start_if_cnt);
        SPRINTF_CHECK_VOID
        (*start_if_cnt)++;
    }

    if (is_if)
    {
        //начальный
        //условие для финального прыжка
        sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%sIF_RES%d:\n", asm_code, *start_if_cnt);
        SPRINTF_CHECK_VOID

        //закрытие условия
        sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%sIF_COND%d:\n", asm_code, *if_cnt);
            SPRINTF_CHECK_VOID

        if (IS_ELSE_IF)
            code_if(node, ids, asm_code, list); 
        
        else if (IS_ELSE)
            get_part(node->Right, ids, str, list);  //предполагается возможность дальнейшего разветвления
    
        RETURN_VOID
    }

//-------expression--------------
    if (IS_EQUAL)
        code_equal(node, asm_code, list);
    

    //эти штуки нужны для условий. Это просто для того, чтобы поставить правильный джамп
    //так что можно выделить эти три операции отдельно, а код писать в зависимости от полученного результата
    //это и решит проблему с тем, что надо писать в условном лэйбле ифа
    else if (IS_GREATER)
    {

    }
    else if (IS_LESS)
    {

    }

//-----cycles----
    if (IS_WHILE)
    {

    }
    else if (IS_FOR)
    {

    }
    else
        code_op(node, ids, asm_code, list);  //printf, особые операции
          
    RETURN_VOID
    
    return;
}

void code_if(Node* node, size_t *const if_cnt, Id *const ids, char *const str, ErrList *const list)
{
    assert(node);
    assert(str);
    assert(list);
    assert(if_cnt);
    assert(ids);

    get_part(node->Left, ids, str, list);  //предполагаются equal, greater less
    RETURN_VOID
    
    int sprintf_res = sprintf_s(str, MAX_FILE_SIZE, "%s IF_COND%d:\n", str, *if_cnt);  //может, надо сделат типа массив лэйблов для таких штук
    SPRINTF_CHECK_VOID

    (*if_cnt)++;

    get_part(node->Right, ids, str, list);  //предполагается возможность дальнейшего разветвления
    RETURN_VOID
}

void code_else_if(Node* node, size_t *const if_cnt, Id *const ids, char *const str, ErrList *const list)
{
    assert(node);
    assert(str);
    assert(list);
    assert(if_cnt);
    assert(ids);


}

void code_equal(Node* node, char *const str, ErrList *const list)
{
    assert(node);
    assert(str);
    assert(list);

    if (node->Right->type == NUM || node->Right->type == ID)
        node_push(node->Right, str, list);
    else
        code_op(node->Right, ids, str, list);
    RETURN_VOID

    node_pop(node->Left, str, list);
    RETURN_VOID
}


void code_op(Node* node, Id *const ids, char *const str, ErrList *const list)
{
    assert(node);
    assert(str);
    assert(list);

    node_push(node->Left, str, list);
    RETURN_VOID

    node_push(node->Right, str, list);
    RETURN_VOID
    
    for (int i = 0; i < APPR_CMD; i++)
    {
        if (appropriate_cmds[i].num == node->value)
        {
            int sprintf_res = sprintf_s(str, MAX_STR_LEN, "%s%s\n", str, appropriate_cmds[i].cmd.cmd_str);
            SPRINTF_CHECK_VOID
            break;
        }
    }
}

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
