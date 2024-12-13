#include <stdio.h>

#include "assembly.h"

static void fill_labels(Labels *const labels, Input *const asm_text, Stack *const functions, ErrList *const list);
static bool find_label_mark (const char* const str);
static bool find_arg (const char *const str);
static size_t find_label(Labels* labels, const char *const str);

static void find_cmd_num(char *const str, size_t* cmd, ErrList *const list);

static void handle_commands(Labels *const labels, Input *const asm_text, Stack* new_buf, ErrList *const list);
static void handle_args (Labels *const labels, Input *const asm_text, const size_t cmd, Stack* new_buf, ErrList *const list);
static bool complicated_arg_case (Stack* new_buf, Input *const asm_text, char *const str, ErrList *const list);

static void arg_analysis (Input *const asm_text, int *const arg1, int *const arg2, ErrList *const list);
static void RAM_case (int *const arg1, int *const arg2, Input *const asm_text, ErrList *const list);
static void plus_case(size_t *const cmd_num, Input *const asm_text, int* arg2, ErrList *const list);

static bool find_register (const char *const str, int *const arg);

static void print_binary_int (int a);

void assembly (Input *const asm_text, Labels *const labels, Stack* new_buf, Stack* functions, ErrList *const list)
{
    assert(asm_text);
    assert(labels);
    assert(new_buf);

    fill_labels(labels, asm_text, functions, list);
    RETURN_VOID

    for (int i = 0; i < LABELS_AMT; i++);
        printf("lab name %s\n", labels->labels->name);

    handle_commands(labels, asm_text, new_buf, list);
    RETURN_VOID
}

//----------------------LABELS---------------------------------------------------------------------------------------

void ctor_labels(Labels* labels, ErrList *const list)
{
    assert(labels);
    assert(list);

    LabelParameters* label = (LabelParameters*)calloc(LABELS_AMT, sizeof(LabelParameters));
    ALLOCATION_CHECK_VOID(labels)

    for (size_t i = 0; i < LABELS_AMT; i++)
        label[i].target = START_VALUE;

    labels->labels = label;
}

void dtor_labels(Labels* labels)
{
    free(labels->labels);
}

void fill_labels(Labels *const labels, Input *const asm_text, Stack *const functions, ErrList *const list)
{
    assert(labels);
    assert(asm_text);
    assert(functions);
    assert(list);

    char** buf = asm_text->addresses;
    bool is_label = false;
    bool is_arg  = false;
    size_t label_ind = 0;
    char str[MAX_STR_LEN] = {};

    for (size_t word_cnt = 0; word_cnt < asm_text->cmd_amt; word_cnt++)
    {
        strncpy(str, buf[word_cnt], MAX_STR_LEN);
        CPY_CHECK(str)

        is_label = find_label_mark(str);

        if (!is_label)
            continue;

        label_ind = find_label(labels, str);
        if (label_ind == ERROR_VALUE_SIZE_T)
        {
            ERROR(ALLOCATION_ERROR)
            return;
        }

        is_arg = find_arg(buf[word_cnt - 1]);

        int cmp_res = strncmp(RET_STR, buf[word_cnt], MAX_STR_LEN);

        if (!is_arg && label_ind != START_VALUE &&  cmp_res != 0)
        {
            labels->labels[label_ind].target = (int)word_cnt;  //может быть разный размер слова, разное количество бит, в побитовом файле в таком случае могут возникнуть проблемы, это стоит учитывать
            labels->labels[label_ind].name = buf[word_cnt];
        }
        else if (is_arg && cmp_res == 0)
        {
            stk_push(functions, word_cnt, list);
            RETURN_VOID
        }
            
        is_label = false;
    } 
}

//------------------------COMMANDS-----------------------------------------------------------------------------------


void handle_commands(Labels *const labels, Input *const asm_text, Stack* new_buf, ErrList *const list)
{
    assert(labels);
    assert(asm_text);
    assert(new_buf);
    assert(list);

    int sscanf_check = 0;

    size_t cmd_amt = asm_text->cmd_amt;
    char str [MAX_STR_LEN] = {};
    char* file_buf = {};
    bool is_label = false;
    size_t cmd = 0;
    int sscanf_res = 0;
    bool is_compl = false;

    for (size_t cmd_num = 0; cmd_num < cmd_amt; cmd_num++)
    {
        asm_text->cmd_num = cmd_num;
        labels->label_type = LABEL_DEF;
        is_label = false;
        
        file_buf = asm_text->addresses[cmd_num];
        sscanf_res = sscanf(file_buf, "%s", str);
        SSCANF_CHECK

        is_label = find_label_mark(str);

        if (is_label)
            continue;

        find_cmd_num(str, &cmd, list);

        stk_push(new_buf, (stack_element_t)cmd, list);  //выглядит немного лажова, понятно, что можно явно приводить типы, но это вообще принято или кирнж?
        RETURN_VOID

        is_compl = complicated_arg_case(new_buf, asm_text, str, list);
        
        if (is_compl)
        {
            cmd_num = asm_text->cmd_num;
            continue;
        }

        handle_args(labels, asm_text, cmd, new_buf, list);
    }
}



//---------------------------------------ARGS---------------------------------------------------------------------

void handle_args (Labels *const labels, Input *const asm_text, const size_t cmd, Stack* new_buf, ErrList *const list)
{
    assert(labels);
    assert(asm_text);
    assert(new_buf);
    assert(list);

    int arg = 0;
    char str[MAX_STR_LEN] = {};
    bool is_label = false;
    size_t ind = 0;
    int sscanf_res = 0;

    size_t cmd_num = asm_text->cmd_num;
    size_t args_amt = bunch_of_commands[cmd].arg_amt;
    int sscanf_check = 0;

    for (size_t arg_num = 0; arg_num < args_amt; arg_num++)
    {
        is_label = false;
        cmd_num++; 
        char* file_buf = asm_text->addresses[cmd_num];
        sscanf_res = sscanf(file_buf, "%s", str);
        SSCANF_CHECK

        is_label = find_label_mark(str);
        if (is_label)
        {
            ind = find_label(labels, str);
            stk_push(new_buf, labels->labels[ind].target, list);
            RETURN_VOID
        }         
        else
        {
            sscanf_res = sscanf(file_buf, "%d", &arg);
            SSCANF_CHECK
            stk_push(new_buf, arg, list);
            RETURN_VOID
        }
    }
}

bool complicated_arg_case (Stack* new_buf, Input *const asm_text, char *const str, ErrList *const list)  //надо еще посмотреть, что тут с cmd_num
{
    assert(new_buf);
    assert(asm_text);
    assert(str);
    assert(list);

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

    arg_analysis(asm_text, &arg1, &arg2, list);
    RETURN_BOOL

    if (arg1 != POISON)
    {
        stk_push(new_buf, arg1, list);
        RETURN_BOOL
    }
    
    if (arg2 != POISON)
    {
        stk_push(new_buf, arg2, list);
        RETURN_BOOL
    }
        

    return is_compl;
}

void arg_analysis (Input *const asm_text, int *const arg1, int *const arg2, ErrList *const list)
{
    assert(asm_text);
    assert(arg1);
    assert(arg2);
    assert(list);

    size_t cmd_num = asm_text->cmd_num;
    char str[MAX_STR_LEN] = {};
    cmd_num++;
    char* file_buf = asm_text->addresses[cmd_num];
    int sscanf_res = sscanf (file_buf, "%s", str); 
    SSCANF_CHECK

    bool is_register = find_register(str, arg1);

    bool is_int = false;
    if (sscanf_res != 0)
        is_int = true;

    if (strncmp(str, BRACE, MAX_STR_LEN) == 0)
    {
        asm_text->cmd_num = cmd_num;
        RAM_case(arg1, arg2, asm_text, list);
        RETURN_VOID
        cmd_num = asm_text->cmd_num;
    }
    else if (is_register)
    {
        *arg1 |= REG;
    }
    else if (is_int)
    {
        sscanf_res = sscanf(file_buf, "%d", arg1);
        SSCANF_CHECK

        *arg1 |= INT;
    }

    asm_text->cmd_num = cmd_num;
}

void RAM_case (int *const arg1, int *const arg2, Input *const asm_text, ErrList *const list)
{
    assert(asm_text);
    assert(arg1);
    assert(arg2);
    assert(list);

    *arg1 |= RAM;
    *arg2 |= RAM;
    
    size_t cmd_num = asm_text->cmd_num;
    char str[MAX_STR_LEN] = {};
    
    cmd_num++;
    char* file_buf = asm_text->addresses[cmd_num];
    int sscanf_res = sscanf (file_buf, "%s", str);
    SSCANF_CHECK
    
    bool is_register = find_register(str, arg1);

    if (is_register)
    {
        *arg1 |= REG;
    }
    else
    {
        sscanf_res = sscanf(file_buf, "%d", arg1);
        SSCANF_CHECK
        *arg1 |= INT;
        
        cmd_num++;
        file_buf = asm_text->addresses[cmd_num];

        sscanf_res = sscanf (file_buf, "%s", str);
        SSCANF_CHECK

        if (strncmp(str, PLUS, MAX_STR_LEN) == 0)
        {
            plus_case(&cmd_num, asm_text, arg2, list);
            RETURN_VOID
            *arg2 |= REG;
        }
    }

    cmd_num++;

    file_buf = asm_text->addresses[cmd_num];

    sscanf_res = sscanf (file_buf, "%s", str); 
    SSCANF_CHECK

    asm_text->cmd_num = cmd_num;
}

void plus_case(size_t *const cmd_num, Input *const asm_text, int* arg2, ErrList *const list)
{
    assert(cmd_num);
    assert(asm_text);
    assert(arg2);
    assert(list);

    char str[MAX_STR_LEN] = {};
    char* file_buf = asm_text->addresses[*cmd_num];

    (*cmd_num)++;
    file_buf = asm_text->addresses[*cmd_num];
    int sscanf_res = sscanf (file_buf, "%s", str);
    SSCANF_CHECK

    find_register(str, arg2);
}


void make_binary_file (Stack *const new_buf, ErrList *const list)
{
    assert(new_buf);
    assert(list);

    int* stack_buf = new_buf->data;
    int* file_buf = (int*)calloc(new_buf->capacity, sizeof(int));
    ALLOCATION_CHECK_VOID(file_buf)

    int file_size = 0;

    while(stack_buf[file_size] != POISON)
    {
        file_buf[file_size] = stack_buf[file_size];
        file_size++;
    }

    FILE* output_file;
    output_file = fopen("out.txt", "wb");
    FILE_CHECK(output_file)

    int write_res = fwrite (file_buf, sizeof(int), file_size, output_file);
    WRITE_CHECK    

    int close_res = fclose(output_file);
    CLOSE_CHECK

    free(file_buf);
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
void find_cmd_num(char *const str, size_t* cmd, ErrList *const list)
{
    assert(str);
    assert(cmd);
    assert(list);

    for (size_t cmd_num = 0; cmd_num < CMD_AMT; cmd_num++)
    {
        if (strcmp(str, bunch_of_commands[cmd_num].cmd_str) == 0)
        {
            *cmd = (size_t)bunch_of_commands[cmd_num].cmd_num;
        }
        else if (cmd_num > CMD_AMT - 1)
        {
            //printf("there is no such command\n");
            ERROR(SYN_ERROR)
            return;
        }
    }
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

        if (ch == LABEL_MARK && i == strlen(str) - 1)
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

    return ERROR_VALUE_SIZE_T;
}