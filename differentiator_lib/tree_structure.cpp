#include <stdio.h>

#include "tree_structure.h"

Node* node_ctor ()
{
	Node* node = (Node*)calloc(1, sizeof(Node));

	return node;  //проверка осуществляется вне функции
}

Node* make_node(Type type, double value, Node* Left, Node* Right, Err_param *const error)
{
	assert(error);

	Node* node = node_ctor();
	ALLOCATION_CHECK_RET(node)
	
	node->type = type;
	node->value = value;
	node->Left = Left;
	node->Right = Right;

	return node;
}

void tree_ctor (Tree *const the_tree, Node *const start_node)
{
    assert(the_tree);
	assert(start_node);

    the_tree->root = start_node;
}

Node* tree_dtor(Node* node)
{
	if (!node->Left && !node->Right)
	{
		free(node);
		node = nullptr;
		return node;
	}	

	if (node->Left)
	{
		tree_dtor(node->Left);

		if (!node->Right)
		{
			free(node);
			node = nullptr;
		}
	}

	if (node->Right)
	{
		tree_dtor(node->Right);
		free(node);
		node = nullptr;
	}

	return node;
}


Node* node_dtor(Node* node)
{
	free(node);
	node = nullptr;

	return node;
}