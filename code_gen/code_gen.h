#ifndef _GEN_H_
#define _GEN_H_

#include "..\proc_lib\asm_struct.h"
#include "..\syn_analysis\syn_analysis.h"

#define IS_SEP node->type == OP && node->value == SEP

#define IS_IF node->type == OP && node->value == IF

#define IS_EQUAL node->type == OP && node->value == EQUAL

static const size_t APPR_CMD = 7;
static const char* ASM_NAME = "asm_file.txt";

struct CodeOp
{
    CommandParameters cmd;
    Operation num;
};

const struct CodeOp ADD_CMD  =  {AddStr, ADD};
const struct CodeOp SUB_CMD  =  {SubStr, SUB};
const struct CodeOp MUL_CMD  =  {MulStr, MUL};
const struct CodeOp DIV_CMD  =  {DivStr, DIV};
const struct CodeOp SIN_CMD  =  {SinStr, SIN};
const struct CodeOp COS_CMD  =  {CosStr, COS};
const struct CodeOp END_CMD  =  {HltStr, END};

static const CodeOp appropriate_cmds[APPR_CMD] = {ADD_CMD,
                                                  SUB_CMD,
                                                  MUL_CMD,
                                                  DIV_CMD,
                                                  SIN_CMD,
                                                  COS_CMD,
                                                  END_CMD};

void code_gen(Tree *const the_tree, ErrList *const list);
#endif //_GEN_H_