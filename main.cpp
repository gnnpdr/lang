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
//обработа текста - разбивание на токены
	printf("TEXT AN\n");
	root = analyse_text(tokens, ids, &base_text, &list);
	/*MAIN
//-----------------------------------
	Tree the_tree = {};
	tree_ctor (&the_tree, root);
	graph_dump(root, ids, root, &list);
//-------------------------------------------
	// генерация ассемблерного кода
	code_gen(&the_tree, &list);
	MAIN

	*/input_dtor(&base_text);
	tokens_dtor(tokens);
	ids_dtor(ids);/*
	tree_dtor(the_tree.root);
///-----------------------

	Input asm_text = {};
    input_ctor(&asm_text, &list);
	MAIN

    handle_text_wname (&asm_text, ASM_NAME, &list);
	MAIN
	
	Word* words = word_list_ctor(&list);
	// заполнение массива слов
	get_code(&asm_text, words, &list);

	LabelParameters* labels = ctor_labels(&list);
	FuncParameters* funcs = ctor_funcs(&list);
	
    Stack stk_code = {};
    stk_ctor(&stk_code, &list);
	MAIN

	// переход от ассемблерного кода к цифровому
    assembly(words, labels, funcs, &stk_code, &list);
	MAIN
	
	dtor_labels(labels);

	// обработка цифрового кода
	Input bin_code = {};
    input_ctor(&bin_code, &list);
	MAIN

	handle_text_wname (&bin_code, BIN_FILE_NAME, &list);
	MAIN
	
	Proc proc = {};
	proc_ctor(&proc, &list);
	MAIN
	
	get_bin_code(&bin_code, &proc, &list);
	
	Stack prog = {};
    stk_ctor(&prog, &list);
	MAIN
	
	proc_code(&proc, &prog, &list);  // надо добавить регистры, некоторые функции типа вывод аргумента

	proc_dtor(&proc);
	stk_dtor(&prog);

	input_dtor(&asm_text);
	word_list_dtor(words);
    stk_dtor(&stk_code);*/

	error_list_dtor(&list);
}