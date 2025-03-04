#ifndef _GEN_H_
#define _GEN_H_

#include "..\proc_lib\asm_struct.h"
#include "..\syn_analysis\syn_analysis.h"

#define IS_SEP node->type == OP && node->value == SEP

#define IS_IF node->type == OP && node->value      == IF
#define IS_ELSE_IF node->type == OP && node->value == ELSE_IF
#define IS_ELSE node->type == OP && node->value    == ELSE

#define IS_EQUAL node->type == OP && node->value         == EQUAL
#define IS_GREATER node->type == OP && node->value       == GREATER
#define IS_LESS node->type == OP && node->value          == LESS
#define IS_GREATER_EQUAL node->type == OP && node->value == GREATER_EQUAL
#define IS_LESS_EQUAL node->type == OP && node->value    == LESS_EQUAL

#define IS_WHILE node->type == OP && node->value == WHILE
#define IS_FOR node->type == OP && node->value   == FOR

#define IS_PRINTF node->type == OP && node->value == PRINTF
#define IS_SQRT node->type == OP && node->value   == SQRT

static const size_t ARITHM_CMD_AMT = 4;

struct CodeOp
{
    CommandParameters cmd;
    Operation num;
};

const struct CodeOp ADD_CMD  =  {AddStr, ADD};
const struct CodeOp SUB_CMD  =  {SubStr, SUB};
const struct CodeOp MUL_CMD  =  {MulStr, MUL};
const struct CodeOp DIV_CMD  =  {DivStr, DIV};

static const CodeOp ArithmCmds[ARITHM_CMD_AMT]    =    {ADD_CMD,
                                                        SUB_CMD,
                                                        MUL_CMD,
                                                        DIV_CMD};

void code_gen(Tree *const the_tree, Id *const ids, ErrList *const list);

#endif //_GEN_H_