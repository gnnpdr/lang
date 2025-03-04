#include <stdio.h>

#include "assembly.h"

static void get_labels(Word *const words, LabelParameters *const labels, ErrList *const list);
static bool find_label_mark(char *const word, size_t len);
static size_t find_label (LabelParameters *const labels, char *const word, size_t len);
static size_t handle_cmds(Word *const words, LabelParameters *const labels, FuncParameters *const funcs, Stack *const stk_code, ErrList *const list);
static void handle_args(size_t word_num, Word word, LabelParameters *const labels, Stack *const stk_code, ErrList *const list);
static void fill_bin_file(const char* const  input_file_name, size_t size, const int* const input_file_data, ErrList *const list);
static void fill_labels(Word *const words, LabelParameters *const labels, Stack *const stk_code, ErrList *const list);

static size_t find_func (FuncParameters *const funcs, char *const word, size_t len);
static void get_funcs(Word *const words, FuncParameters *const funcs, ErrList *const list);

static size_t find_func_by_ret_num(size_t word_num, FuncParameters* funcs, ErrList *const list);

//-------------COMMON---------------------

void assembly(Word *const words, LabelParameters *const labels, FuncParameters *const funcs, Stack *const stk_code, ErrList *const list)
{
    ASM_ASSERT

    get_labels(words, labels, list);
    get_funcs(words, funcs, list);

    handle_cmds(words, labels, funcs, stk_code, list);        
    stk_code->size = 0;
    size_t dig_amt = handle_cmds(words, labels, funcs, stk_code, list);    
    
    fill_labels(words, labels, stk_code, list);
    
    int* bin_code = (int*)calloc(dig_amt, sizeof(int));
    
    for (size_t i = 0; i < dig_amt; i++)
        bin_code[i] = stk_code->data[i];

    printf("ENTER NAME FOR YOUR BIN FILE\n");
    char bin_file_name[MAX_STR_LEN] = {};
    scanf("%s", bin_file_name);

    fill_bin_file(bin_file_name, dig_amt, bin_code, list);

    free(bin_code);
}

//-------------------LABELS-------------------------------------------------------------------------------

//----------------make and other--------------------------------------------------------------------------

LabelParameters* ctor_labels(ErrList *const list)
{
    assert(list);

    LabelParameters* labels = (LabelParameters*)calloc(LABELS_AMT, sizeof(LabelParameters));
    ALLOCATION_CHECK_PTR(labels)

    for (size_t i = 0; i < LABELS_AMT; i++)
    {
        size_t* arg_target = (size_t*)calloc(LABEL_ARG_AMT, sizeof(size_t));
        ALLOCATION_CHECK_PTR(labels)

        for (size_t arg = 0; arg < LABEL_ARG_AMT; arg++)
            arg_target[arg] = ERROR_VALUE_SIZE_T;

        labels[i].cmd_target = ERROR_VALUE_SIZE_T;
        labels[i].arg_target = arg_target;
        labels[i].len = ERROR_VALUE_SIZE_T;
    }

    return labels;
}

void dtor_labels(LabelParameters *const labels)
{
    for (size_t arg = 0; arg < LABEL_ARG_AMT; arg++)
        free(labels[arg].arg_target);

    free(labels);
}

bool find_label_mark(char *const word, size_t len)
{
    assert(word);

    if (word[len - 1] == LABEL_MARK)
        return true;
    else
        return false;
}

size_t find_label (LabelParameters *const labels, char *const word, size_t len)
{
    assert(labels);
    assert(word);

    size_t lab_num = ERROR_VALUE_SIZE_T;

    for (size_t num = 0; num < LABELS_AMT; num++)
    {
        if (labels[num].len == ERROR_VALUE_SIZE_T)
        {
            lab_num = num;
            break;
        }

        int cmp_res = strncmp(word, labels[num].start_word, len);

        if (cmp_res == 0)
        {
            lab_num = num;
            break;
        }
    }

    return lab_num;
}

//--------------in code-------------------------

void get_labels(Word *const words, LabelParameters *const labels, ErrList *const list)
{
    assert(words);
    assert(labels);
    assert(list);

    size_t word = 0;
    bool is_label = false;
    size_t label = 0;

    while (words[word].len != ERROR_VALUE_SIZE_T)
    {
        is_label = find_label_mark(words[word].word_start, words[word].len);

        if(is_label)
        {
            label = find_label(labels, words[word].word_start, words[word].len);

            if (label == ERROR_VALUE_SIZE_T)
            {
                ERROR(ALLOCATION_ERROR)
                return;
            }

            labels[label].len = words[word].len;
            labels[label].start_word = words[word].word_start;

            if (words[word].type == CMD)
            {
                words[word].type = LABEL_CMD;
            }
            else
                words[word].type = LABEL_ARG;
        }

        is_label = false;
        word++;

    }
}

void fill_labels(Word *const words, LabelParameters *const labels, Stack *const stk_code, ErrList *const list)
{
    assert(words);
    assert(labels);
    assert(stk_code);
    assert(list);

    size_t label = 0;
    stack_element_t* data = stk_code->data;

    while (labels[label].len != ERROR_VALUE_SIZE_T)
    {
        size_t num = 0;
        while (labels[label].arg_target[num] != ERROR_VALUE_SIZE_T)
        {
            data[labels[label].arg_target[num]] = labels[label].cmd_target;
            num++;
        }
    
        label++;
    }
}

//----------------------FUNCS----------------------

//----------------make and other----------------------------

FuncParameters* ctor_funcs(ErrList *const list)
{
    assert(list);

    FuncParameters* funcs = (FuncParameters*)calloc(FUNCS_AMT, sizeof(FuncParameters));
    ALLOCATION_CHECK_PTR(funcs)

    for (size_t i = 0; i < FUNCS_AMT; i++)
    {
        funcs[i].len = ERROR_VALUE_SIZE_T;
        funcs[i].call_target = ERROR_VALUE_SIZE_T;
        funcs[i].ret_word = ERROR_VALUE_SIZE_T;
    }
        
    return funcs;
}

void dtor_funcs(FuncParameters *const funcs)
{
    free(funcs);
}

size_t find_func (FuncParameters *const funcs, char *const word, size_t len)
{
    assert(funcs);
    assert(word);

    size_t func_num = ERROR_VALUE_SIZE_T;

    for (size_t num = 0; num < FUNCS_AMT; num++)
    {
        if (funcs[num].len == ERROR_VALUE_SIZE_T)
        {
            func_num = num;
            break;
        }

        int cmp_res = strncmp(word, funcs[num].start_word, len);

        if (cmp_res == 0)
        {
            func_num = num;
            break;
        }
    }

    return func_num;
}

//--------------in code-------------------------

void get_funcs(Word *const words, FuncParameters *const funcs, ErrList *const list)
{
    assert(words);
    assert(funcs);
    assert(list);

    size_t word = 0;
    size_t func_num = 0;
    size_t func_amt = 0;

    while (words[word].len != ERROR_VALUE_SIZE_T)
    {
        int cmp_res = strncmp(words[word].word_start, CALL_STR, strlen(CALL_STR));

        if(cmp_res == 0)
        {
            word++;
            func_num = find_func(funcs, words[word].word_start, words[word].len);

            if (funcs[func_num].len == ERROR_VALUE_SIZE_T)
            {
                funcs[func_num].start_word = words[word].word_start;
                funcs[func_num].len = words[word].len;
                func_amt++;
            }
        }
        word++;
    }
}


size_t find_func_by_ret_num(size_t word_num, FuncParameters* funcs, ErrList *const list)
{
    for (size_t func_num = 0; func_num < FUNCS_AMT; func_num++)
    {
        if (funcs[func_num].ret_word == word_num)
            return func_num;
    }
    ERROR(SYN_ERROR)
    return ERROR_VALUE_SIZE_T;
}

//----------------------CMDS-----------------------

size_t handle_cmds(Word *const words, LabelParameters *const labels, FuncParameters *const funcs, Stack *const stk_code, ErrList *const list)
{
    ASM_ASSERT

    size_t word = 0;
    size_t label = 0;
    size_t dig_amt = 0;

    while (words[word].len != ERROR_VALUE_SIZE_T)
    {
        if (words[word].type == LABEL_CMD)
        {
            label = find_label(labels, words[word].word_start, words[word].len);
            labels[label].cmd_target = dig_amt - 1;
            word++;
            continue;
        }
            
        size_t cmd_num = ERROR_VALUE_SIZE_T;
        
        for (size_t cmd = 0; cmd < CMD_AMT; cmd++)
        {
            int cmp_res = strncmp(words[word].word_start, bunch_of_commands[cmd].cmd_str, words[word].len);

            if (cmp_res == 0)
            {
                cmd_num = cmd;
                break;
            }
        }
        
        if (cmd_num == ERROR_VALUE_SIZE_T)
        {
            size_t func_num = find_func(funcs, words[word].word_start, words[word].len);

            if (func_num == ERROR_VALUE_SIZE_T)
            {
                printf("ERROR CMD\n");
                ERROR(SYN_ERROR)
                return ERROR_VALUE_SIZE_T;
            }
            else
            {
                funcs[func_num].call_target = dig_amt;
                word++;
                continue;
            }
        }

        dig_amt++;

        if (bunch_of_commands[cmd_num].cmd_num == CALL_A)
        {
            stk_push(stk_code, CALL_A, list);
            word++;

            size_t func_num = find_func(funcs, words[word].word_start, words[word].len);
            stk_push(stk_code, funcs[func_num].call_target, list);
            dig_amt++;

            word++;

            continue;
        }
        else if (bunch_of_commands[cmd_num].cmd_num == RET_A)
        {
            stk_push(stk_code, RET_A, list);
            word++;
            continue;
        }

        stk_push(stk_code, bunch_of_commands[cmd_num].cmd_num, list);
        

        for (size_t arg = 0; arg < bunch_of_commands[cmd_num].arg_amt; arg++)
        {
            word++;
            dig_amt += 2;
            
            handle_args(dig_amt, words[word], labels, stk_code, list);
        }
          
        word++;
    }
    return dig_amt;
}

//-----------ARGS------------------------

void handle_args(size_t dig_num, Word word, LabelParameters *const labels, Stack *const stk_code, ErrList *const list)
{
    assert(labels);
    assert(stk_code);
    assert(list);

    int arg = ERROR_VALUE_INT;
    
    if (isdigit(word.word_start[0]) || word.word_start[0] == '-')
    {
        arg = atoi(word.word_start); 
        stk_push(stk_code, ARG_NUM, list);
        stk_push(stk_code, arg, list);
    }
    else if(word.word_start[0] == RAM_MARK)
    {
        arg = atoi(word.word_start + 1);
        stk_push(stk_code, ARG_RAM, list);
        stk_push(stk_code, arg, list);
    }
    else if(word.type == LABEL_ARG)
    {
        stk_push(stk_code, ARG_LAB, list);
        stk_push(stk_code, ERROR_VALUE_INT, list);

        size_t label = find_label(labels, word.word_start, word.len);

        size_t num = 0;
        while (labels[label].arg_target[num] != ERROR_VALUE_SIZE_T)
            num++;

        labels[label].arg_target[num] = dig_num - 1;
    }
}

//-------------------------BIN_FILE--------------------

void fill_bin_file(const char* const  input_file_name, size_t size, const int* const input_file_data, ErrList *const list)
{
    assert(input_file_name);
    assert(input_file_data);
    assert(list);
    
    FILE* output_file;
    
    output_file = fopen(input_file_name, "w");
    
    FILE_CHECK(output_file)
    
    for (size_t i = 0; i < size; i++)
        fprintf(output_file, "%d ", input_file_data[i]);
        
    int close_res = fclose(output_file);
    CLOSE_CHECK
}