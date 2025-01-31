#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_


//------------------------CONST-----------------------------------

static const size_t VAR_AMT =  1;
static const size_t OP_AMT  =  18;

static const double START_VALUE =  -1;
static const char ID_MARK       = '*';
static const char END_MARK      = '@';
static const char* BR_STR    	= "!";
static const char* CALC_BR_STR  = "&";
static const char* SEP_STR  	= ".";


static const char* ADD_STR   	= "darim";
static const char* SUB_STR   	= "grabim";
static const char* MUL_STR   	= "kniga";
static const char* DIV_STR   	= "religia";
static const char* KOREN_STR 	= "koren";

static const char* END_STR   	= "@1991";

static const char* IF_STR    	= "revolution";
static const char* ELSE_IF_STR  = "inache_revolution";
static const char* ELSE_STR    	= "sovsem_inache";

static const char* EQUAL_STR 	= "ravenstvo";
static const char* GREATER_STR 	= "RSDRPB";
static const char* LESS_STR 	= "PSDRPM";

static const char* PRINTF_STR 	= "slovo";

static const char* WHILE_STR 	= "do_teh_por";
static const char* FOR_STR 		= "v_techenie";


//-----------------------ENUM-------------------------------------

enum Type
{
	NUM = 0,
	ID = 1,
	OP  = 2
};

enum Operation
{
	SEP   	= 0,
	CALC_BR = 1,
	BR    	= 2,
	ADD   	= 3,
	SUB   	= 4,
	MUL   	= 5,
	DIV   	= 6,
	SQRT 	= 7,
	END   	= 8,
	IF    	= 9,
	ELSE_IF = 10,
	ELSE 	= 11,
    EQUAL 	= 12,
    GREATER = 13,
	LESS 	= 14,
	PRINTF 	= 15,
	WHILE 	= 16,
	FOR 	= 17
};

//----------------------STRUCT-----------------------------------

struct Node
{
	Type type;
	int value;

	Node* Left;
	Node* Right;
};

struct Tree
{
	Node* root;
};

struct OperationParameters
{
    const char* name;
    Operation num;
};

static const struct OperationParameters SEP_STRUCT   	= {SEP_STR     , SEP     };
static const struct OperationParameters CALC_BR_STRUCT  = {CALC_BR_STR ,  CALC_BR};
static const struct OperationParameters BR_STRUCT    	= {BR_STR      ,  BR     };
static const struct OperationParameters ADD_STRUCT   	= {ADD_STR     ,  ADD    };
static const struct OperationParameters SUB_STRUCT   	= {SUB_STR     ,  SUB    };
static const struct OperationParameters MUL_STRUCT   	= {MUL_STR     ,  MUL    };
static const struct OperationParameters DIV_STRUCT   	= {DIV_STR     ,  DIV    };
static const struct OperationParameters SQRT_STRUCT  	= {KOREN_STR   ,  SQRT   };
static const struct OperationParameters END_STRUCT   	= {END_STR     ,  END    };
static const struct OperationParameters IF_STRUCT    	= {IF_STR      ,  IF     };
static const struct OperationParameters ELSE_IF_STRUCT 	= {ELSE_IF_STR ,  ELSE_IF};
static const struct OperationParameters ELSE_STRUCT    	= {ELSE_STR    ,  ELSE   };
static const struct OperationParameters EQUAL_STRUCT 	= {EQUAL_STR   ,  EQUAL  };
static const struct OperationParameters GREATER_STRUCT  = {GREATER_STR ,  GREATER};
static const struct OperationParameters LESS_STRUCT    	= {LESS_STR    ,  LESS   };
static const struct OperationParameters PRINTF_STRUCT   = {PRINTF_STR  ,  PRINTF };
static const struct OperationParameters WHILE_STRUCT    = {WHILE_STR   ,  WHILE  };
static const struct OperationParameters FOR_STRUCT    	= {FOR_STR     ,  FOR    };

static const struct OperationParameters* operations [OP_AMT] = {&SEP_STRUCT,
																&CALC_BR_STRUCT,
																&BR_STRUCT ,
																&ADD_STRUCT,
																&SUB_STRUCT,
																&MUL_STRUCT,
																&DIV_STRUCT,
																&SQRT_STRUCT,
																&END_STRUCT,
																&IF_STRUCT,
																&ELSE_IF_STRUCT,
																&ELSE_STRUCT,
                                                                &EQUAL_STRUCT,
																&GREATER_STRUCT,
																&LESS_STRUCT,
																&PRINTF_STRUCT,
																&WHILE_STRUCT,
																&FOR_STRUCT};

#endif //_STRUCTURE_H_