#include <stdio.h>

#include "lex_analysis\lex_analysis.h"
#include "syn_analysis\syn_analysis.h"
#include "code_gen\code_gen.h"


int main(int argc, char** argv)
{
    ErrList list = {};
	error_list_ctor(&list);
	MAIN

	Input base_text = {};
    input_ctor(&base_text, &list);
	MAIN
    get_database_name(&base_text, argv, &list);
	MAIN
    get_database_text(&base_text, &list);
	MAIN

	Node* root = node_ctor(&list);
	MAIN

	Token* tokens = tokens_ctor(&list);
	MAIN
    Id* ids = id_ctor(&list);
	MAIN

	lex_analysis(tokens, ids, &base_text, &list);
	MAIN
	root = syn_analysis(tokens, ids, &list);
	MAIN

	Tree the_tree = {};
	tree_ctor (&the_tree, root);

	code_gen(&the_tree, &list);
	MAIN

	tokens_dtor(tokens);
	ids_dtor(ids);
	tree_dtor(the_tree.root);
	input_dtor(&base_text);
	error_list_dtor(&list);
}