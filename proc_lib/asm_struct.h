#ifndef _DATA_H_
#define _DATA_H_

//static const int    ERROR_VALUE    = -8;

//-----------------------COMMANDS---------------------------------------------------------------------------

static const size_t CMD_AMT  =  19;


enum ArgType
{
    ARG_NUM, 
    ARG_LAB,
    ARG_RAM,
    ARG_REG,
    ARG_CALL
};

enum CommandsNums
{
    PUSH_A  = 1 ,
    ADD_A   = 2 ,
    SUB_A   = 3 ,
    MUL_A   = 4 ,
    DIV_A   = 5 ,
    SIN_A   = 6 ,
    COS_A   = 7 ,
    OUT_A   = 8 ,
    HLT_A   = 9 ,
    POP_A   = 10,
    JA_A    = 11,
    JAE_A   = 12,
    JB_A    = 13,
    JBE_A   = 14,
    JE_A    = 15,
    JNE_A   = 16,
    JMP_A   = 17,
    CALL_A  = 18,
    RET_A   = 19
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
const struct CommandParameters SinStr   =  {"sin"  , SIN_A  , 1};
const struct CommandParameters CosStr   =  {"cos"  , COS_A  , 1};
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
const struct CommandParameters CallStr  =  {"call" , CALL_A , 1};
const struct CommandParameters RetStr   =  {"ret"  , RET_A  , 0};

static const CommandParameters bunch_of_commands [CMD_AMT]  =   {PushStr,
                                                                AddStr   ,
                                                                SubStr   ,
                                                                MulStr   ,
                                                                DivStr   ,
                                                                SinStr   ,
                                                                CosStr   ,
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
                                                                RetStr   };



//----------------------------------LABELS---------------------------------------------------------------------

static const size_t LABELS_AMT = 10;
static const size_t RET_AMT    = 10;

static const char LABEL_MARK = ':';

struct LabelParameters
{
    size_t arg_target;
    size_t cmd_target;
    char* start_word;
    size_t len;
};

//--------------------------------FUNCS-------------------------------------

static const size_t FUNCS_AMT = 10;

struct FuncParameters
{
    char* start_word;
    size_t len;

    size_t var_amt; //чтобы сдвигаться на положенное количество ячеек, если функция будет вызвана не один раз
    size_t* vars;  //массив новеров ячеек оперативной памяти, которые используются функцией. Последняя - возвращаемое значение
    int* ret_array;   //int на случай, если код слишком большой и error_value_size_t не будет неправильным
    size_t call_target;  //чтобы потом поменять код, надо знать, где менять, для этого отмечаются здесь и в лэйблах, targetы
    size_t ret_target;  //заполняться это место будет числом из массива

    size_t ret_word;
};

//--------------------------------REGISTERS---------------------------------------------------------------
/*
static const size_t REG_AMT = 4;

enum Registers
{
    AX = 0,
    BX = 1,
    CX = 2,
    DX = 3
};

struct RegisterParameters
{
    int value;
    const char* name;
    size_t num;
};

static const char* AX_STR = "ax";
static const char* BX_STR = "bx";
static const char* CX_STR = "cx";
static const char* DX_STR = "dx";

static struct RegisterParameters ax = {ERROR_VALUE, AX_STR, AX};
static struct RegisterParameters bx = {ERROR_VALUE, BX_STR, BX};
static struct RegisterParameters cx = {ERROR_VALUE, CX_STR, CX};
static struct RegisterParameters dx = {ERROR_VALUE, DX_STR, DX};

static struct RegisterParameters* registers [REG_AMT] = {&ax,
                                                         &bx,
                                                         &cx,
                                                         &dx};
*/
//--------------------------------ARGS----------------------------------------------------------------

static const size_t MK_ARGS_STRS  =  6;
static const size_t COMPL_ARG_AMT =  2;
static const size_t RAM_AMT       = 50;

static const char* PLUS  = "+";
static const char RAM_MARK = '[';
//static const char RAM_MARK_C = ']';

static const int BITS_IN_BYTES = 8;
static const int INT_BYTE_SIZE = sizeof(int)*BITS_IN_BYTES;

/*enum ArgType
{
    INT  =   1 << (sizeof(int)*BITS_IN_BYTES - 1) ,
    REG  =   1 << (sizeof(int)*BITS_IN_BYTES - 2) ,
    RAM  =   1 << (sizeof(int)*BITS_IN_BYTES - 3) ,
    FREE = ~(7 << (sizeof(int)*BITS_IN_BYTES - 3)),
};*/

static const char* MK_ARGS[6] = {
                                 JA_STR,
                                 JAE_STR,
                                 JB_STR,
                                 JBE_STR,
                                 JE_STR,
                                 JNE_STR
                                };

static const char* COMPL_ARG[COMPL_ARG_AMT] =  {
                                                POP_STR,
                                                PUSH_STR
                                               };

#endif //_DATA_H_