#ifndef _GEN_H_
#define _GEN_H_

#include "..\proc_lib\assembly.h"

#define IS_SEP node->type == OP && node->value == SEP

#define IS_IF node->type == OP && node->value == IF

#define IS_EQUAL node->type == OP && node->value == EQUAL

static const size_t APPR_CMD = 7;

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
/*const struct CodeOp JaStr    =  {.cmd_str = "ja"   , .cmd_num = JA   , .arg_amt = 1};
const struct CodeOp JaeStr   =  {.cmd_str = "jae"  , .cmd_num = JAE  , .arg_amt = 1};
const struct CodeOp JbStr    =  {.cmd_str = "jb"   , .cmd_num = JB   , .arg_amt = 1};
const struct CodeOp JbeStr   =  {.cmd_str = "jbe"  , .cmd_num = JBE  , .arg_amt = 1};
const struct CodeOp JeStr    =  {.cmd_str = "je"   , .cmd_num = JE   , .arg_amt = 1};
const struct CodeOp JneStr   =  {.cmd_str = "jne"  , .cmd_num = JNE  , .arg_amt = 1};*/
//const struct CodeOp JmpStr   =  {.cmd_str = "jmp"  , .cmd_num = JMP  , .arg_amt = 1};
//const struct CodeOp RetStr   =  {.cmd_str = "ret"  , .cmd_num = RET  , .arg_amt = 0};

static const CodeOp appropriate_cmds[APPR_CMD] = {ADD_CMD,
                                                  SUB_CMD,
                                                  MUL_CMD,
                                                  DIV_CMD,
                                                  SIN_CMD,
                                                  COS_CMD,
                                                  END_CMD};

void code_gen(Tree *const the_tree, ErrList *const list);
#endif //_GEN_H_