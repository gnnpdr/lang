#include <stdio.h>

#include "lex_analysis\lex_analysis.h"
#include "syn_analysis\syn_analysis.h"
#include "code_gen\code_gen.h"


int main(int argc, char** argv)
{
    ERROR_CTOR

	Input base_text = {};
    input_ctor(&base_text, &error);
    get_database_name(&base_text, argv, &error);
    get_database_text(&base_text, &error);

	Node* root = node_ctor();

	Token* tokens = tokens_ctor(&error);
    Id* ids = id_ctor(&error);

	lex_analysis(tokens, ids, &base_text, &error);
	root = syn_analysis(tokens, ids, &error);

	Tree the_tree = {};
	tree_ctor (&the_tree, root);

	code_gen(&the_tree, &error);

	tokens_dtor(tokens);
	ids_dtor(ids);
	tree_dtor(the_tree.root);
	input_dtor(&base_text);
}