#include <stdio.h>

#include "lex_analysis\lex_analysis.h"
#include "syn_analysis\syn_analysis.h"

int main(int argc, char** argv)
{
    ERROR_CTOR

	Input base_text = {};
    input_ctor(&base_text, &error);
    get_database_name(&base_text, argv, &error);
    get_database_text(&base_text, &error);

    //printf("TEXT\n%s\n", base_text.text);

	Node* root = node_ctor();
	MAIN_CHECK  //сделать проверку на nullptr и выделить ей отдельный макрос

	Token* tokens = tokens_ctor(&error);
    MAIN_CHECK 
    Id* ids = id_ctor(&error);
    MAIN_CHECK 

	lex_analysis(tokens, ids, &base_text, &error);
	root = syn_analysis(tokens, ids, &error);

	Tree the_tree = {};
	tree_ctor (&the_tree, root);
	//graph_dump(root,ids, root, &error);

	tokens_dtor(tokens);
	ids_dtor(ids);
	tree_dtor(the_tree.root);
	input_dtor(&base_text);
}