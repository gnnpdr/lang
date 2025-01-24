#include <stdio.h>

#include "code_gen.h"

static void get_part(Node *const node, char* asm_code, ErrList *const list);

static void code_if(Node* node, char *const str, ErrList *const list);
static void code_equal(Node* node, char *const str, ErrList *const list);
static void code_op(Node* node, char *const str, ErrList *const list);

static void node_push(Node* node, char *const str, ErrList *const list);
static void node_pop(Node* node, char *const str, ErrList *const list);

void code_gen(Tree *const the_tree, ErrList *const list)
{
    assert(the_tree);
    assert(list);

    char* asm_code = (char*)calloc(MAX_FILE_SIZE, sizeof(char));
    ALLOCATION_CHECK_VOID(asm_code)

    Node* root = the_tree->root;
    get_part(root, asm_code, list);
    RETURN_VOID
    int sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s%s\n", asm_code, HLT_STR);
    SPRINTF_CHECK_VOID

    fill_input_file(ASM_NAME, asm_code, list);
    //printf("here\n");
    //RETURN_VOID
    free(asm_code);
    
}

void get_part(Node *const node, char* asm_code, ErrList *const list)
{
    assert(node);
    assert(asm_code);
    assert(list);

    if (IS_SEP)
    {
        //это лажово, но я еще подумаю над тем, как бы разделить эти штуки пробелами красиво :(
        get_part(node->Left, asm_code, list);
        RETURN_VOID
        int sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s\n", asm_code);
        SPRINTF_CHECK_VOID
        get_part(node->Right, asm_code, list);
        RETURN_VOID
        sprintf_res = sprintf_s(asm_code, MAX_FILE_SIZE, "%s\n", asm_code);
        SPRINTF_CHECK_VOID
    }
    
    if (IS_IF)
        code_if(node, asm_code, list); 

    else if (IS_EQUAL)
        code_equal(node, asm_code, list);
    
    else
        code_op(node, asm_code, list);
          
    RETURN_VOID
    
    return;
}

void code_if(Node* node, char *const str, ErrList *const list)
{  
    //здесь будет обрабатываться равенство нулю (пока тока это, но это легко исправить легкой рукой в грамматиеи)
    assert(node);
    assert(str);
    assert(list);

    code_op(node->Left, str, list);
    RETURN_VOID
    
    int sprintf_res = sprintf_s(str, MAX_FILE_SIZE, "%s%s 0\n%s IF:\n", str, PUSH_STR, JNE_STR);  //может, надо сделат типа массив лэйблов для таких штук
    SPRINTF_CHECK_VOID

    code_equal(node->Right, str, list);
    RETURN_VOID

    sprintf_res = sprintf_s(str, MAX_FILE_SIZE, "%sIF:\n", str, PUSH_STR, JNE_STR);
    SPRINTF_CHECK_VOID
}

void code_equal(Node* node, char *const str, ErrList *const list)
{
    assert(node);
    assert(str);
    assert(list);

    if (node->Right->type == NUM || node->Right->type == ID)
        node_push(node->Right, str, list);
    else
        code_op(node->Right, str, list);
    RETURN_VOID

    node_pop(node->Left, str, list);
    RETURN_VOID
}

void code_op(Node* node, char *const str, ErrList *const list)
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
    {
        //или надо сделать в процессоре типа переменные как бы отвечающие за ячейки оп памяти?
        sprintf_res = sprintf_s(str, MAX_FILE_SIZE, "%s%s [%d]\n", str, PUSH_STR, node->value); 
    }
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
