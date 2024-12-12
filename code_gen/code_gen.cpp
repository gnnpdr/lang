#include <stdio.h>

#include "code_gen.h"

static char* get_part(Node *const node, char* asm_code, Err_param *const error);

static void code_if(Node* node, char *const str, Err_param *const error);
static void code_equal(Node* node, char *const str, Err_param *const error);
static void code_op(Node* node, char *const str, Err_param *const error);

static void node_push(Node* node, char *const str, Err_param *const error);
static void node_pop(Node* node, char *const str, Err_param *const error);

void code_gen(Tree *const the_tree, Err_param *const error)
{
    char asm_code[MAX_FILE_SIZE] = {};

    Node* root = the_tree->root;
    get_part(root, asm_code, error);
    sprintf_s(asm_code, MAX_FILE_SIZE, "%s%s", asm_code, HLT_STR);

    printf("%s\n", asm_code);

}

char* get_part(Node *const node, char* asm_code, Err_param *const error)
{
    assert(node);
    assert(asm_code);
    assert(error);

    if (IS_SEP)
    {

        //это лажово, но я еще подумаю над тем, как бы разделить эти штуки пробелами красиво :(
        get_part(node->Left, asm_code, error);
        sprintf_s(asm_code, MAX_FILE_SIZE, "%s\n", asm_code);
        get_part(node->Right, asm_code, error);
        sprintf_s(asm_code, MAX_FILE_SIZE, "%s\n", asm_code);
    }
    
    if (IS_IF)
    {
        code_if(node, asm_code, error); 
        //sprintf_s(asm_code, MAX_FILE_SIZE, "%s\n", asm_code);
    }
    else if (IS_EQUAL)
    {
        //printf("equal\n");
        code_equal(node, asm_code, error);
    }
    else
    {
        code_op(node, asm_code, error);
    }        

    

    return asm_code;
}

void code_if(Node* node, char *const str, Err_param *const error)
{  //здесь будет обрабатываться равенство нулю (пока)

    code_op(node->Left, str, error);
    
    sprintf_s(str, MAX_FILE_SIZE, "%s%s 0\n%s IF:\n", str, PUSH_STR, JNE_STR);  //может, надо сделат типа массив лэйблов для таких штук
     
    code_equal(node->Right, str, error);

    sprintf_s(str, MAX_FILE_SIZE, "%sIF:\n", str, PUSH_STR, JNE_STR);
}

void code_equal(Node* node, char *const str, Err_param *const error)
{
    if (node->Right->type == NUM || node->Right->type == ID)
        node_push(node->Right, str, error);
    else
        code_op(node->Right, str, error);

    node_pop(node->Left, str, error);
}

void code_op(Node* node, char *const str, Err_param *const error)
{
    assert(node);
    assert(error);

    node_push(node->Left, str, error);
    node_push(node->Right, str, error);
    
    for (int i = 0; i < APPR_CMD; i++)
    {
        if (appropriate_cmds[i].num == node->value)
        {
            sprintf_s(str, MAX_STR_LEN, "%s%s\n", str, appropriate_cmds[i].cmd.cmd_str);
            break;
        }
    }
}

void node_push(Node* node, char *const str, Err_param *const error)
{
    assert(node);
    assert(error);


    if (node->type == NUM)
    {
        sprintf_s(str, MAX_FILE_SIZE, "%s%s %d\n", str, PUSH_STR, node->value);
    }
    else if(node->type == ID)
    {
        //или надо сделать в процессоре типа переменные как бы отвечающие за ячейки оп памяти?
        sprintf_s(str, MAX_FILE_SIZE, "%s%s [%d]\n", str, PUSH_STR, node->value); 
    }
}

void node_pop(Node* node, char *const str, Err_param *const error)
{
    assert(node);
    assert(error);

    char part[MAX_STR_LEN] = {};

    if (node->type == NUM)
    {
        sprintf_s(str, MAX_FILE_SIZE, "%s%s %d\n", str, POP_STR, node->value);
    }
    else if(node->type == ID)
    {
        sprintf_s(str, MAX_FILE_SIZE, "%s%s [%d]\n", str, POP_STR, node->value);
    }
}