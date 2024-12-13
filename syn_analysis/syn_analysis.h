#ifndef _SYN_H_
#define _SYN_H_

#include "..\draw\draw_tree.h"

static const size_t FIRST_PR_AMT = 2;
static const size_t SEC_PR_AMT = 3;
static const size_t UNARY_OP_AMT = 4;
static const double EXP = 2.72;

static const struct OperationParameters first_pr [FIRST_PR_AMT]  = {ADD_STRUCT,
															        SUB_STRUCT};

static const struct OperationParameters sec_pr [SEC_PR_AMT] = {MUL_STRUCT,
															   DIV_STRUCT};

static const struct OperationParameters unary_op [UNARY_OP_AMT]  = {SIN_STRUCT,
																    COS_STRUCT};

#define REC_ASSERT 	do						\
					{						\
						assert(tokens);		\
    					assert(ids);		\
    					assert(pointer);	\
    					assert(list);		\
					}while(0);

#define BR_CHECK  	do 																		\
					{																		\
						if (tokens[*pointer].type != OP && tokens[*pointer].value != BR) 	\
					    {																	\
					        printf("you dont have a brace at char number %d\n", *pointer);	\
					        ERROR(SYN_ERROR)												\
					        return nullptr;													\
					    }																	\
						(*pointer)++;														\
					}while(0);

Node* syn_analysis(Token *const tokens, Id *const ids, ErrList *const list);

#endif //_SYN_H_