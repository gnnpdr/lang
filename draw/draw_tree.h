#ifndef _DRAW_H_
#define _DRAW_H_

#include "../handle_expr/get_expr.h"

static const char* OP_DEF  = "operation";
static const char* NUM_DEF = "number";
static const char* VAR_DEF = "variable";

void graph_dump(Node* const node, Id *const ids, Node* const definite_node, ErrList *const list);
void fill_input_file(const char* const  input_file_name, const char* const input_file_data, ErrList *const list);

#endif //_DRAW_H_