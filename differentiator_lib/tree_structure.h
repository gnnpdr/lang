#ifndef _STRUCT_H_
#define _STRUCT_H_

//-----------------------LIBS-------------------------------------
#include "..\errors\errors.h"
#include "..\structure.h"

#include <assert.h>
#include <stdlib.h>

//----------------------FUNCS--------------------------------------
Node* node_ctor ();
void tree_ctor (Tree *const the_tree, Node *const start_node);
Node* make_node(Type type, double value, Node* Left, Node* Right, Err_param *const error);

Node* tree_dtor(Node* node);
Node* node_dtor(Node* node);

#endif //_STRUCT_H_