#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_


//------------------------CONST-----------------------------------

static const size_t VAR_AMT =  1;
static const size_t OP_AMT  =  14;

static const double START_VALUE =  0;
static const char ID_MARK = '*';
static const char END_MARK = '@';

static const char* ADD_STR   = "darim";
static const char* SUB_STR   = "grabim";
static const char* MUL_STR   = "kniga";
static const char* DIV_STR   = "religia";
static const char* SIN_STR   = "serp";
static const char* COS_STR   = "molot";
static const char* END_STR   = "@1991";
static const char* BR_STR    = "!";
static const char* IF_STR    = "revolution";
static const char* EQUAL_STR = "ravenstvo";
static const char* SEP_STR  = ".";
static const char* STOP1_STR = "#";
static const char* STOP2_STR = "$";
static const char* STOP3_STR = "%";

//-----------------------ENUM-------------------------------------

enum Type
{
	NUM = 0,
	ID = 1,
	OP  = 2
};

enum Operation
{
	ADD   = 0,
	MUL   = 1,
	DIV   = 2,
	SUB   = 3,
	SIN   = 4,
	COS   = 5,
	END   = 6,
	BR    = 7,
    EQUAL = 8,
    IF    = 9,
	SEP   = 10,
	STOP1 = 11,
	STOP2 = 12,
	STOP3 = 13
};

//----------------------STRUCT-----------------------------------

//правда нужен юнион? мне же нужно все это
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

static const struct OperationParameters ADD_STRUCT   = {ADD_STR  ,  ADD };
static const struct OperationParameters MUL_STRUCT   = {MUL_STR  ,  MUL };
static const struct OperationParameters DIV_STRUCT   = {DIV_STR  ,  DIV };
static const struct OperationParameters SUB_STRUCT   = {SUB_STR  ,  SUB };
static const struct OperationParameters SIN_STRUCT   = {SIN_STR  ,  SIN };
static const struct OperationParameters COS_STRUCT   = {COS_STR  ,  COS };
static const struct OperationParameters END_STRUCT   = {END_STR  ,  END };
static const struct OperationParameters BR_STRUCT    = {BR_STR   ,  BR  };
static const struct OperationParameters EQUAL_STRUCT = {EQUAL_STR, EQUAL};
static const struct OperationParameters SEP_STRUCT   = {SEP_STR  , SEP  };
static const struct OperationParameters ST1_STRUCT   = {STOP1_STR, STOP1};
static const struct OperationParameters ST2_STRUCT   = {STOP2_STR, STOP2};
static const struct OperationParameters ST3_STRUCT   = {STOP3_STR, STOP3};
static const struct OperationParameters IF_STRUCT    = {IF_STR   ,  IF  };

static const struct OperationParameters* operations [OP_AMT] = {&ADD_STRUCT,
																&MUL_STRUCT,
																&DIV_STRUCT,
																&SUB_STRUCT,
																&SIN_STRUCT,
																&COS_STRUCT,
																&END_STRUCT,
																&BR_STRUCT ,
                                                                &EQUAL_STRUCT,
                                                                &IF_STRUCT,
																&SEP_STRUCT,
																&ST1_STRUCT,
																&ST2_STRUCT,
																&ST3_STRUCT};



#endif //_STRUCTURE_H_