#ifndef _DATA_H_
#define _DATA_H_

//-----------------------COMMANDS---------------------------------------------------------------------------

static const size_t CMD_AMT  =  18;


enum ArgType
{
    ARG_NUM, 
    ARG_LAB,
    ARG_RAM,
    ARG_CALL
};

enum CommandsNums
{
    PUSH_A  = 1 ,
    ADD_A   = 2 ,
    SUB_A   = 3 ,
    MUL_A   = 4 ,
    DIV_A   = 5 ,
    OUT_A   = 6 ,
    HLT_A   = 7 ,
    POP_A   = 8 ,
    JA_A    = 9 ,
    JAE_A   = 10,
    JB_A    = 11,
    JBE_A   = 12,
    JE_A    = 13,
    JNE_A   = 14,
    JMP_A   = 15,
    CALL_A  = 16,
    RET_A   = 17,
    SQRT_A  = 18
};

static const char* POP_STR  =  "pop";
static const char* PUSH_STR = "push";

static const char* JA_STR = "ja";
static const char* JE_STR = "je";
static const char* JB_STR = "jb";

static const char* JAE_STR = "jae";
static const char* JNE_STR = "jne";
static const char* JBE_STR = "jbe";
static const char* JMP_STR = "jmp";

static const char* RET_STR = "ret";
static const char* HLT_STR = "hlt";

static const char* CALL_STR = "call";

static const char* SQRT_STR = "sqrt";

struct CommandParameters 
{
    const char*  cmd_str;
    CommandsNums cmd_num;
    size_t       arg_amt;
};

const struct CommandParameters PushStr  =  {"push" , PUSH_A , 1};
const struct CommandParameters AddStr   =  {"add"  , ADD_A  , 0};
const struct CommandParameters SubStr   =  {"sub"  , SUB_A  , 0};
const struct CommandParameters MulStr   =  {"mul"  , MUL_A  , 0};
const struct CommandParameters DivStr   =  {"div"  , DIV_A  , 0};
const struct CommandParameters HltStr   =  {"hlt"  , HLT_A  , 0};
const struct CommandParameters OutStr   =  {"out"  , OUT_A  , 0};
const struct CommandParameters PopStr   =  {"pop"  , POP_A  , 1};
const struct CommandParameters JaStr    =  {"ja"   , JA_A   , 1};
const struct CommandParameters JaeStr   =  {"jae"  , JAE_A  , 1};
const struct CommandParameters JbStr    =  {"jb"   , JB_A   , 1};
const struct CommandParameters JbeStr   =  {"jbe"  , JBE_A  , 1};
const struct CommandParameters JeStr    =  {"je"   , JE_A   , 1};
const struct CommandParameters JneStr   =  {"jne"  , JNE_A  , 1};
const struct CommandParameters JmpStr   =  {"jmp"  , JMP_A  , 1};
const struct CommandParameters CallStr  =  {"call" , CALL_A , 0};
const struct CommandParameters RetStr   =  {"ret"  , RET_A  , 0};
const struct CommandParameters SqrtStr  =  {"sqrt" , SQRT_A , 0};

static const CommandParameters bunch_of_commands [CMD_AMT]  =   {PushStr,
                                                                AddStr   ,
                                                                SubStr   ,
                                                                MulStr   ,
                                                                DivStr   ,
                                                                HltStr   ,
                                                                OutStr   ,
                                                                PopStr   ,
                                                                JaStr    ,
                                                                JaeStr   ,
                                                                JbStr    ,
                                                                JbeStr   ,
                                                                JeStr    ,
                                                                JneStr   ,
                                                                JmpStr   ,
                                                                CallStr  ,
                                                                RetStr   ,
                                                                SqrtStr};

//----------------------------------LABELS---------------------------------------------------------------------

static const char LABEL_MARK = ':';

static const size_t LABELS_AMT    = 10;
static const size_t LABEL_ARG_AMT = 5;

struct LabelParameters
{
    char* start_word;
    size_t len;

    size_t cmd_target;
    size_t* arg_target;
};

//--------------------------------FUNCS-------------------------------------

static const size_t FUNCS_AMT = 10;

struct FuncParameters
{
    char* start_word;
    size_t len;

    size_t cmd_target;
    size_t call_target;

    size_t ret_word;
};

//--------------------------------ARGS----------------------------------------------------------------

static const char* PLUS    = "+";
static const char RAM_MARK = '[';

#endif //_DATA_H_