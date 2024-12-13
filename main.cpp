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

	printf("root address %p\n", root);

	Token* tokens = tokens_ctor(&list);
	MAIN
    Id* ids = id_ctor(&list);
	MAIN

	printf("made\n");

	analyse_text(tokens, ids, &base_text, &list);
	MAIN

	printf("fuck\n");
	Tree the_tree = {};
	tree_ctor (&the_tree, root);
	printf("you are funny\n");
	code_gen(&the_tree, &list);
	MAIN
	printf("give me a paper\n");
	input_dtor(&base_text);
	tokens_dtor(tokens);
	ids_dtor(ids);
	tree_dtor(the_tree.root);

	Input asm_text = {};
    input_ctor(&asm_text, &list);
	MAIN
    handle_text_wname (&asm_text, ASM_NAME, &list);
	MAIN

	Labels labels = {};
    Stack functions = {};
    stk_ctor(&functions, &list);
	MAIN
    ctor_labels(&labels, &list);
	MAIN

    Stack new_buf = {};
    stk_ctor(&new_buf, &list);
	MAIN

    assembly(&asm_text, &labels, &new_buf, &functions, &list);
	MAIN
    dtor_labels(&labels);

    Stack stk = {};
    stk_ctor(&stk, &list);
	MAIN

	Processor proc = {};
    proc.new_file_buf = new_buf.data;
    proc.ncmd = new_buf.size;
    proc_file (&stk, &proc, &functions, &list);
	MAIN

	input_dtor(&asm_text);
    stk_dtor(&stk);
    stk_dtor(&new_buf);
    stk_dtor(&functions);

	error_list_dtor(&list);
}