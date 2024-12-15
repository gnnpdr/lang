#include <stdio.h>

#include "lex_analysis\lex_analysis.h"
#include "syn_analysis\syn_analysis.h"
#include "code_gen\code_gen.h"
#include "proc_lib\input.h"
#include "proc_lib\stk.h"
#include "proc_lib\assembly.h"
#include "proc_lib\proc.h"

int main(int argc, char** argv)
{
    ErrList list = {};
	error_list_ctor(&list);
	MAIN

	Input base_text = {};
    input_ctor(&base_text, &list);
	MAIN
    get_text(&base_text, argv, &list);
	MAIN

	Node* root = node_ctor(&list);
	MAIN

	Token* tokens = tokens_ctor(&list);
	MAIN
    Id* ids = id_ctor(&list);
	MAIN

	root = analyse_text(tokens, ids, &base_text, &list);
	MAIN
	
	Tree the_tree = {};
	
	tree_ctor (&the_tree, root);
	graph_dump(root, ids, root, &list);


	code_gen(&the_tree, &list);
	//printf("88\n");
	MAIN
	input_dtor(&base_text);
	tokens_dtor(tokens);
	ids_dtor(ids);
	tree_dtor(the_tree.root);

	Input asm_text = {};
    input_ctor(&asm_text, &list);

	//Wors
	//printf("52\n");
	//printf("text %p\naddresses %p\n text->text %p\n", &asm_text, asm_text.addresses, asm_text.text);
	MAIN
    handle_text_wname (&asm_text, ASM_NAME, &list);
	MAIN
	Word* words = word_list_ctor(&list);
	get_code(&asm_text, words, &list);

	LabelParameters* labels = ctor_labels(&list);
	//printf("here!!\n");
    Stack stk_code = {};
    stk_ctor(&stk_code, &list);
	//printf("now_here\n");
	MAIN
	
    assembly(words, labels, &stk_code, &list);
	MAIN
	
	dtor_labels(labels);

    /*Stack stk = {};
    stk_ctor(&stk, &list);
	MAIN

	Processor proc = {};
    proc.new_file_buf = new_buf.data;
    proc.ncmd = new_buf.size;
    proc_file (&stk, &proc, &functions, &list);
	MAIN*/


	input_dtor(&asm_text);
	word_list_dtor(words);
    stk_dtor(&stk_code);
    /*stk_dtor(&new_buf);
    stk_dtor(&functions);*/

	error_list_dtor(&list);
}