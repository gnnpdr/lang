/*#include <stdio.h>

#include "assembly.h"

static Errors fill_labels(Labels *const labels, Text *const input, Stack *const functions);
static bool find_label_mark (const char* const str);
static bool find_arg (const char *const str);
static size_t find_label(Labels* labels, const char *const str);

static Errors find_cmd_num(char *const str, size_t* cmd);

static Errors handle_commands(Labels *const labels, Text *const input, Stack* new_buf);
static Errors handle_args (Labels *const labels, Text *const input, const size_t cmd, Stack* new_buf);
static bool complicated_arg_case (Stack* new_buf, Text *const input, char *const str);

static Errors arg_analysis (Text *const input, int *const arg1, int *const arg2);
static Errors RAM_case (int *const arg1, int *const arg2, Text *const input);
static Errors plus_case(size_t* cmd_num, Text* input, int* arg2);

static bool find_register (const char *const str, int *const arg);

static void print_binary_int (int a);

void assembly (Text *const input, Labels *const labels, Stack* new_buf, Stack* functions)
{
    assert(input  );
    assert(labels );
    assert(new_buf);

    fill_labels(labels, input, functions);

    handle_commands(labels, input, new_buf);
}

//----------------------LABELS---------------------------------------------------------------------------------------

Errors ctor_labels(Labels* labels)
{
    assert(labels);

    LabelParameters* label = (LabelParameters*)calloc(LABELS_AMT, sizeof(LabelParameters));
    ALLOCATION_CHECK(labels);

    for (size_t i = 0; i < LABELS_AMT; i++)
        label[i].target = START_VALUE;

    labels->labels = label;

    return ALL_RIGHT;
}

void dtor_labels(Labels* labels)
{
    free(labels->labels);
}

Errors fill_labels(Labels *const labels, Text *const input, Stack *const functions)
{
    assert(labels);
    assert(input );
    assert(functions);

    char** buf = input->addresses;
    bool is_label = false;
    bool is_arg  = false;
    size_t label_ind = 0;
    char str[MAX_STR_LEN] = "";

    for (size_t word_cnt = 0; word_cnt < input->cmd_amt; word_cnt++)
    {

        strncpy(str, buf[word_cnt], MAX_STR_LEN);
        if(str == nullptr)
            return CPY_ERROR;
        
        is_label = find_label_mark(str);

        if (!is_label)
            continue;

        label_ind = find_label(labels, str);
        if (label_ind == ERROR_VALUE)
            return LABEL_ERROR;

        is_arg = find_arg(buf[word_cnt - 1]);

        int cmp_res = strncmp(RET_STR, buf[word_cnt], MAX_STR_LEN);

        if (!is_arg && label_ind != START_VALUE &&  cmp_res != 0)
        {
            labels->labels[label_ind].target = (int)word_cnt;  //может быть разный размер слова, разное количество бит, в побитовом файле в таком случае могут возникнуть проблемы, это стоит учитывать
            labels->labels[label_ind].name = buf[word_cnt];
        }
        else if (is_arg && cmp_res == 0)
        {
            stk_push(functions, word_cnt);
        }
            

        is_label = false;
    }

   return ALL_RIGHT;    
}

//------------------------COMMANDS-----------------------------------------------------------------------------------


Errors handle_commands(Labels *const labels, Text *const input, Stack* new_buf)
{
    assert(labels );
    assert(input  );
    assert(new_buf);

    int sscanf_check = 0;

    size_t cmd_amt = input->cmd_amt;
    char str [MAX_STR_LEN] = {};
    char* file_buf = {};
    bool is_label = false;
    size_t cmd = 0;
    bool is_compl = false;

    for (size_t cmd_num = 0; cmd_num < cmd_amt; cmd_num++)
    {
        input->cmd_num = cmd_num;
        labels->label_type = LABEL_DEF;
        is_label = false;
        
        file_buf = input->addresses[cmd_num];
        sscanf(file_buf, "%s", str);
        SSCANF_CHECK

        is_label = find_label_mark(str);

        if (is_label)
            continue;

        find_cmd_num(str, &cmd);

        stk_push(new_buf, (stack_element_t)cmd);

        is_compl = complicated_arg_case(new_buf, input, str);
        
        if (is_compl)
        {
            cmd_num = input->cmd_num;
            continue;
        }

        handle_args(labels, input, cmd, new_buf);
    }
    
    return ALL_RIGHT; 
}



//---------------------------------------ARGS---------------------------------------------------------------------

Errors handle_args (Labels *const labels, Text *const input, const size_t cmd, Stack* new_buf)
{
    assert(labels);
    assert(input);
    assert(new_buf);

    int arg = 0;
    char str[MAX_STR_LEN] = {};
    bool is_label = false;
    size_t ind = 0;

    size_t cmd_num = input->cmd_num;
    size_t args_amt = bunch_of_commands[cmd].arg_amt;
    int sscanf_check = 0;

    for (size_t arg_num = 0; arg_num < args_amt; arg_num++)
    {
        is_label = false;
        cmd_num++; 
        char* file_buf = input->addresses[cmd_num];
        sscanf(file_buf, "%s", str);
        SSCANF_CHECK

        is_label = find_label_mark(str);
        if (is_label)
        {
            ind = find_label(labels, str);
            stk_push(new_buf, labels->labels[ind].target);
        }         
        else
        {
            sscanf(file_buf, "%d", &arg);
            SSCANF_CHECK
            stk_push(new_buf, arg);
        }
    }

    return ALL_RIGHT;
}

bool complicated_arg_case (Stack* new_buf, Text *const input, char *const str)  //надо еще посмотреть, что тут с cmd_num
{
    assert(new_buf);
    assert(input  );
    assert(str    );

    bool is_compl = false;

    for (size_t i = 0; i < COMPL_ARG_AMT; i++)
    {
        if (strcmp(str, COMPL_ARG[i]) == 0)
            is_compl = true;
    }

    if (!is_compl)
        return is_compl;
            
    int arg1 = POISON;
    int arg2 = POISON;

    arg_analysis(input, &arg1, &arg2);

    if (arg1 != POISON)
        stk_push(new_buf, arg1);
    
    if (arg2 != POISON)
        stk_push(new_buf, arg2);

    return is_compl;
}

Errors arg_analysis (Text *const input, int *const arg1, int *const arg2)
{
    assert(input);
    assert(arg1 );
    assert(arg2 );

    size_t cmd_num = input->cmd_num;
    char str[MAX_STR_LEN] = {};
    cmd_num++;
    char* file_buf = input->addresses[cmd_num];
    int sscanf_check = sscanf (file_buf, "%s", str); 
    SSCANF_CHECK

    bool is_register = find_register(str, arg1);

    bool is_int = false;
    if (sscanf_check != 0)
        is_int = true;

    if (strncmp(str, BRACE, MAX_STR_LEN) == 0)
    {
        input->cmd_num = cmd_num;
        RAM_case(arg1, arg2, input);
        cmd_num = input->cmd_num;
    }
    else if (is_register)
    {
        *arg1 |= REG;
    }
    else if (is_int)
    {
        sscanf_check = sscanf(file_buf, "%d", arg1);
        SSCANF_CHECK

        *arg1 |= INT;
    }

    input->cmd_num = cmd_num;

    return ALL_RIGHT;
}

Errors RAM_case (int *const arg1, int *const arg2, Text *const input)
{
    assert(input);
    assert(arg1);
    assert(arg2);

    *arg1 |= RAM;
    *arg2 |= RAM;
    
    size_t cmd_num = input->cmd_num;
    char str[MAX_STR_LEN] = {};
    
    cmd_num++;
    char* file_buf = input->addresses[cmd_num];
    int sscanf_check = sscanf (file_buf, "%s", str);
    SSCANF_CHECK
    
    bool is_register = find_register(str, arg1);

    if (is_register)
    {
        *arg1 |= REG;
    }
    else
    {
        sscanf_check = sscanf(file_buf, "%d", arg1);
        SSCANF_CHECK
        *arg1 |= INT;
        
        cmd_num++;
        file_buf = input->addresses[cmd_num];

        sscanf_check = sscanf (file_buf, "%s", str);
        SSCANF_CHECK

        if (strncmp(str, PLUS, MAX_STR_LEN) == 0)
        {
            plus_case(&cmd_num, input, arg2);
            *arg2 |= REG;
        }
    }

    cmd_num++;

    file_buf = input->addresses[cmd_num];

    sscanf_check = sscanf (file_buf, "%s", str); 
    SSCANF_CHECK

    input->cmd_num = cmd_num;

    return ALL_RIGHT;
}

Errors plus_case(size_t* cmd_num, Text* input, int* arg2)
{
    char str[MAX_STR_LEN] = {};
    char* file_buf = input->addresses[*cmd_num];

    (*cmd_num)++;
    file_buf = input->addresses[*cmd_num];
    int sscanf_check = sscanf (file_buf, "%s", str);
    SSCANF_CHECK

    find_register(str, arg2);

    return ALL_RIGHT;
}


Errors make_binary_file (Stack *const new_buf)
{
    int* stack_buf = new_buf->data;
    int* file_buf = (int*)calloc(new_buf->capacity, sizeof(int));
    ALLOCATION_CHECK(file_buf)
    int file_size = 0;

    while(stack_buf[file_size] != POISON)
    {
        file_buf[file_size] = stack_buf[file_size];
        file_size++;
    }

    FILE* output_file;
    output_file = fopen("out.txt", "wb");
    FILE_CHECK(output_file)

    if (fwrite (file_buf, sizeof(int), file_size, output_file) == false)
        return WRITE_ERROR;

    fclose(output_file);
    if (output_file != nullptr)
        return CLOSE_ERROR;

    free(file_buf);

    return ALL_RIGHT;
}

void print_binary_int (int a) //надо убрать ее в более подходящее место. Или удалить
{
    int bin_str[INT_BYTE_SIZE] = {}; 

    for (int i = (sizeof(int)*8 - 1); i >= 0; i--)
    {
        bin_str[i] = (1 & a);
        a >>= 1;
    }

    int k = 0;
    while (k < INT_BYTE_SIZE)
    {
        k++;
        if (k % 4 == 0)
            printf(" ");
    }

    printf("\n");
}

//-------------------------FIND-SOMETHING-----------------------------------------------------------

bool find_register (const char *const str, int *const arg)
{
    assert(str);
    assert(arg);

    bool is_register = false;

    for (size_t i = 0; i < REG_AMT; i++)
    {
        if (strcmp(registers[i]->name, str) == 0)
        {
            is_register = true;
            *arg = (int)registers[i]->num;
            return is_register;
        } 
    }

    //printf("there is no such register\n");

    return is_register;
}

//find_cum_num
Errors find_cmd_num(char *const str, size_t* cmd)
{
    assert(str);
    assert(cmd);

    for (size_t cmd_num = 0; cmd_num < CMD_AMT; cmd_num++)
    {
        if (strcmp(str, bunch_of_commands[cmd_num].cmd_str) == 0)
        {
            *cmd = (size_t)bunch_of_commands[cmd_num].cmd_num;
        }
        else if (cmd_num > CMD_AMT - 1)
        {
            //printf("there is no such command\n");
            return SYN_ERROR;
        }
    }

    return ALL_RIGHT;
}

bool find_label_mark (const char* const str)
{
    assert(str);

    bool is_label = false;

    char  ch = START_CHAR;
    size_t i =  0;

    while (ch != '\0')
    {
        ch = str[i];

        if (ch == LABEL_MARK && i == strlen(str) - 1)  //считается с пробелом. Можно поправить в выявке адресов
            is_label = true;
        
        i++;
    }

    return is_label;
}

bool find_arg (const char *const str)
{
    assert(str);

    int cmp_res = 0;

    for (size_t i = 0; i < MK_ARGS_STRS; i++)
    {
        cmp_res = strncmp(str, MK_ARGS[i], MAX_STR_LEN);
        if (cmp_res == 0)
            return true;
    }

    return false;
}

size_t find_label(Labels* labels, const char *const str)  //ret будет работать в процессоре, а не в ассемблере
{
    assert(labels);
    assert(str);

    for (size_t i = 0; i < LABELS_AMT; i++)
    {
        if (labels->labels[i].name == nullptr || strcmp(labels->labels[i].name, str) == 0)
            return i;
    }

    return ERROR_VALUE;
}*/