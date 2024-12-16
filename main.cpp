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
	MAIN

	input_dtor(&base_text);
	tokens_dtor(tokens);
	ids_dtor(ids);
	tree_dtor(the_tree.root);

	Input asm_text = {};
    input_ctor(&asm_text, &list);
	MAIN

    handle_text_wname (&asm_text, ASM_NAME, &list);
	MAIN
	Word* words = word_list_ctor(&list);
	get_code(&asm_text, words, &list);

	LabelParameters* labels = ctor_labels(&list);
	
    Stack stk_code = {};
    stk_ctor(&stk_code, &list);
	MAIN
	
    assembly(words, labels, &stk_code, &list);
	MAIN
	
	dtor_labels(labels);

	Input bin_code = {};
    input_ctor(&bin_code, &list);
	MAIN

	handle_text_wname (&bin_code, BIN_FILE_NAME, &list);
	MAIN

	Proc proc = {};
	proc_ctor(&proc, &list);
	MAIN
	printf("1\n");
	get_bin_code(&bin_code, &proc, &list);  //надо поставить проверку на EOF и все ок должно пройти
	printf("2\n");
	Stack prog = {};
    stk_ctor(&prog, &list);
	MAIN
	printf("3\n");
	proc_code(&proc, &prog, &list);  // надо добавить регистры, некоторые функции типа вывод аргумента
									 //надо сделать все необходимые штуки для написания функций - ret и тд
	proc_dtor(&proc);
	stk_dtor(&prog);

	input_dtor(&asm_text);
	word_list_dtor(words);
    stk_dtor(&stk_code);

	error_list_dtor(&list);
}