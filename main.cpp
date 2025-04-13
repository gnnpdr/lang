#include <stdio.h>

#include "handle_expr\lex_analysis.h"
#include "handle_expr\syn_analysis.h"
#include "handle_expr\get_expr.h"
#include "code_gen\code_gen.h"
#include "proc_lib\input.h"
#include "proc_lib\assembly.h"
#include "proc_lib\proc.h"

void text_to_asm(char** argv, ErrList *const list);
void asm_to_dig(ErrList *const list);
void proc_dig_code(ErrList *const list);

int main(int argc, char** argv)
{
    ErrList list = {};
	error_list_ctor(&list);
	MAIN

	text_to_asm(argv, &list);
	MAIN

	asm_to_dig(&list);
	MAIN

	proc_dig_code(&list);
	MAIN

	error_list_dtor(&list);

	return 0;
}

void text_to_asm(char** argv, ErrList *const list)
{
	assert(argv);
	assert(list);
	
	Input base_text = {};
    input_ctor(&base_text, list);
	RETURN_VOID

    fill_input(&base_text, argv[1], list);
	RETURN_VOID

	Token* tokens = tokens_ctor(list);
	RETURN_VOID
    Id* ids = id_ctor(list);
	RETURN_VOID
	
	Node* root = node_ctor(list);
	RETURN_VOID
	root = analyse_text(tokens, ids, &base_text, list);
	RETURN_VOID
	
	Tree the_tree = {};
	tree_ctor(&the_tree, root);
	RETURN_VOID
	
	code_gen(&the_tree, ids, list);
	RETURN_VOID
	
	input_dtor(&base_text);
	tree_dtor(root);
	ids_dtor(ids);
	tokens_dtor(tokens);
}

void asm_to_dig(ErrList *const list)
{
	assert(list);

	Input asm_text = {};
    input_ctor(&asm_text, list);
	RETURN_VOID

	char* asm_file_name = (char*)calloc(MAX_STR_LEN, sizeof(char));
	printf("ENTER ASM FILE NAME!\n");

	scanf("%s", asm_file_name);

    fill_input(&asm_text, asm_file_name, list);
	RETURN_VOID

	Word* words = word_list_ctor(list);
	get_code(&asm_text, words, list);

	LabelParameters* labels = ctor_labels(list);
	FuncParameters* funcs = ctor_funcs(list);

    Stack stk_code = {};
    stk_ctor(&stk_code, list);
	RETURN_VOID

    assembly(words, labels, funcs, &stk_code, list);
	RETURN_VOID

	dtor_funcs(funcs);
	dtor_labels(labels);
	word_list_dtor(words);
    stk_dtor(&stk_code);
	input_dtor(&asm_text);
}

void proc_dig_code(ErrList *const list)
{
	assert(list);

	Input bin_code = {};
    input_ctor(&bin_code, list);
	RETURN_VOID

	char* dig_file_name = (char*)calloc(MAX_STR_LEN, sizeof(char));
	printf("ENTER DIG FILE NAME!\n");
	scanf("%s", dig_file_name);
	fill_input(&bin_code, dig_file_name, list);
	RETURN_VOID

	free(dig_file_name);

	int* code = (int*)calloc(MAX_FILE_SIZE, sizeof(int));
	size_t dig_amt = 0;

	get_bin_code(&bin_code, code, &dig_amt, list);
	Proc proc = {};
	proc_ctor(&proc, list);
	RETURN_VOID

	proc.size = dig_amt;
	for (int i = 0; i < dig_amt; i++)
		proc.code[i] = code[i];

	free(code);
	
	Stack prog = {};
    stk_ctor(&prog, list);
	Stack stk = {};
    stk_ctor(&stk, list);
	RETURN_VOID

	proc_code(&proc, &prog, &stk, list);

	stk_dtor(&stk);
	stk_dtor(&prog);
	proc_dtor(&proc);
	input_dtor(&bin_code);
}