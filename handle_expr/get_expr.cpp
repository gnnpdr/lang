#include <stdio.h>
#include "get_expr.h"

static Node* convert_const (Node* node, bool *const is_changed, ErrList *const list, Node* root);
static void calculations(Node *const node, bool *const is_changed);

static Node* check_node_add (Node* node, ErrList *const list, bool *const is_changed);
static Node* check_node_div (Node* node, ErrList *const list, bool *const is_changed);
static Node* check_node_mul(Node* node, ErrList *const list, bool *const is_changed);

static void compare_branches(Node *const node1, Node *const node2, bool *const is_equal);
static bool compare_nodes(Node *const node1, Node *const node2);


Node* analyse_text(Token *const tokens, Id *const ids, Input *const base_text, ErrList *const list)
{
    assert(tokens);
    assert(ids);
    assert(list);
    assert(base_text);

    lex_analysis(tokens, ids, base_text, list);
	RETURN_PTR
    
	Node* root = syn_analysis(tokens, ids, list);
	RETURN_PTR

    bool is_changed = true;
    while (ischanged != false)
    {
        is_changed = false;

        convert_const(root, &is_changed, list, root);

        calculations(root, &is_changed);
    }

    return root;
}


Node* convert_const (Node* node, bool *const is_changed, ErrList *const list, Node* root)
{
    assert(node);
    assert(is_changed);
    assert(list);
    assert(root);

    Node* old_node = node;

    if(node->Left)
        node->Left = convert_const(node->Left, is_changed, list, root);
        
    if(node->Right)
        node->Right = convert_const(node->Right, is_changed, list, root);
        
    if (!node->Right && !node->Left)
        return node;

    if (node->value == ADD || node->value == SUB)
        node = check_node_add(node, list, new_change);
        
    else if (node->value == DIV)
        node = check_node_div(node, list, new_change);
    
    else if (node->value == MUL)
        node = check_node_mul(node, list, new_change);
    RETURN_PTR
    
    return node;
}

Node* check_node_add (Node* node, ErrList *const list, bool *const is_changed)
{
    assert(node);
    assert(list);
    assert(is_changed);

    if (node->Left->value == 0 && node->Left->type == NUM)
    {
        node->Left = node_dtor(node->Left); 
        node = node->Right;
        (*new_change)++;
    }
    else if (node->Right->value == 0 && node->Right->type == NUM)
    {                                                  
        node->Right = node_dtor(node->Right);
        node = node->Left;
        (*new_change)++;
    }
    else if (node->Left->value == 0 && node->Right->value == 0 && node->Right->type == NUM && node->Left->type == NUM)
    {
        node->type = NUM;
        node->value = 0;
        node->Left = node_dtor(node->Left);
        node->Right = node_dtor(node->Right);
        (*new_change)++;
    }
    return node;
}

Node* check_node_div (Node* node, ErrList *const list, bool *const is_changed)
{
    assert(node);
    assert(list);
    assert(is_changed);

    bool is_equal = true;
    compare_branches(node->Left, node->Right, &is_equal);

    if (is_equal)
    {
        node->Right = nullptr; 
        node->Left = nullptr;

        node->type = NUM;
        node->value = 1;
        (*new_change)++;
    }

    if (!node->Left || !node->Right)
        return node;

    if (node->Left->value == 0 && node->Left->type == NUM)
    {
        node->type = NUM;
        node->value = 0;
        
        node->Left = node_dtor(node->Left);
        node->Right = node_dtor(node->Right);
        (*new_change)++;
    }
    else if (node->Right->value == 1 && node->Right->type == NUM)
    {
        node->Right = node_dtor(node->Right);
        node = node->Left;
        (*new_change)++;
    }
    else if (node->Right->value == 0 && node->Right->type == NUM)
    {
        ERROR(MATH_ERROR)
        return nullptr;
    }

    return node;
}

void compare_branches(Node *const node1, Node *const node2, bool *const is_equal)
{
    assert(node1);
    assert(node2);
    assert(is_equal);

    if (node1->Left && node2->Left)
        compare_branches(node1->Left, node2->Left, is_equal);

    if (node1->Right && node2->Right)
        compare_branches(node1->Right, node2->Right, is_equal);

    if (!(*is_equal))
        return;

    *is_equal = compare_nodes(node1, node2);
}

bool compare_nodes(Node *const node1, Node *const node2)
{
    assert(node1);
    assert(node2);

    bool is_equal = false;

    if (node1->type == node2->type && node1->value == node2->value)
        is_equal = true;
    
    return is_equal;
}

Node* check_node_mul(Node* node, ErrList *const list, bool *const is_changed)
{
    assert(node);
    assert(list);
    assert(is_changed);

    if ((node->Left->value == 0 && node->Left->type == NUM) || (node->Right->value == 0 && node->Right->type == NUM))
    {
        node->Left = node_dtor(node->Left);
        node->Right = node_dtor(node->Right);

        node->type = NUM;
        node->value = 0;
        
        (*new_change)++;
    }
    else if (node->Right->value == 1 && node->Right->type == NUM)
    {
        node->Right = node_dtor(node->Left);
        node = node->Left;

        (*new_change)++;
    }
    else if (node->Left->value == 1 && node->Left->type == NUM)
    {
        node->Left = node_dtor(node->Left);
        node = node->Right;
        (*new_change)++;
    } 
    else if (node->Right->type == OP && node->Right->value == DIV)
    {
        Node* new_num_mul = make_node(OP, MUL, node->Right->Left, node->Left, error);

        node->Left = node_dtor(node->Left);

        node->Right->Left = new_num_mul;

        node = node->Right;
        (*new_change)++;
    }
    else if (node->Left->type == OP && node->Left->value == DIV)
    {
        Node* new_num_mul = make_node(OP, MUL, node->Left->Left, node->Right, error);

        node->Right = node_dtor(node->Left);

        node->Left->Left = new_num_mul;

        node = node->Left;
        (*new_change)++;
    }

    return node;
}


void calculations(Node *const node, bool *const is_changed)
{
    assert(node);
    assert(is_changed);

    if(node->Left)
        calculations (node->Left, new_change);

    if(node->Right)
        calculations (node->Right, new_change);
        
    if (node->type == VAR)
        return;
        

    if (!node->Left || !node->Right)
        return;
        
    if (node->Left->type == NUM && node->Right->type == NUM)
    {
        if (node->value == ADD)
        {
            node->type = NUM;
            node->value = node->Left->value + node->Right->value;

            node->Left = node_dtor(node->Left);
            node->Right = node_dtor(node->Right); 
            (*new_change)++;
        }
        else if (node->value == MUL)
        {
            node->type = NUM;
            node->value = node->Left->value * node->Right->value;
               
            node->Left = node_dtor(node->Left);
            node->Right = node_dtor(node->Right); 
            (*new_change)++;
        }
        else if (node->value == DIV)
        {
            node->type = NUM;
            node->value = node->Left->value / node->Right->value;
            
            node->Left = node_dtor(node->Left);
            node->Right = node_dtor(node->Right);
            (*new_change)++;
        }
        else if (node->value == SUB)
        {
            node->type = NUM;
            node->value = node->Left->value - node->Right->value;
               
            node->Left = node_dtor(node->Left);
            node->Right = node_dtor(node->Right);
            (*new_change)++;
        }
    }
}