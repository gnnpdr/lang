#include <stdio.h>

#include "tree_structure.h"

Node* node_ctor (ErrList *const list)
{
	assert(list);

	Node* node = (Node*)calloc(1, sizeof(Node));
	ALLOCATION_CHECK_PTR(node)

	return node;
}

Node* make_node(Type type, double value, Node* Left, Node* Right, ErrList *const list)
{
	assert(list);

	Node* node = node_ctor(list);
	RETURN_PTR
	
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
	assert(node);
	
	if (!node->Left && !node->Right)
	{
        node_dtor(node);
		return node;
	}	

	if (node->Left)
	{
		tree_dtor(node->Left);

		if (node->Right == nullptr)
		{
            node_dtor(node);
		}
	}

	if (node->Right)
	{
		tree_dtor(node->Right);
        node_dtor(node);
	}

	return node;
}


Node* node_dtor(Node* node)
{
	free(node);
	node = nullptr;

	return node;
}