#ifndef _DRAW_H_
#define _DRAW_H_

#include "../lex_analysis/lex_analysis.h"

static const char* OP_DEF = "operation";
static const char* NUM_DEF = "number";
static const char* VAR_DEF = "variable";

static const size_t INPUT_FILE_SIZE = 10000;

void graph_dump(Node* const node, Id *const ids, Node* const definite_node, ErrList *const list);

#endif //_DRAW_H_