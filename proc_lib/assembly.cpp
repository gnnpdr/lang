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
static void fill_func_ret(size_t word_num, FuncParameters func, ErrList *const list);
static void fill_func_args(size_t *const word_num, Word *const words, FuncParameters func, ErrList *const list);
static size_t find_func_by_ret_num(size_t word_num, FuncParameters* funcs, ErrList *const list);


//-------------COMMON---------------------

void assembly(Word *const words, LabelParameters *const labels, FuncParameters *const funcs, Stack *const stk_code, ErrList *const list)
{
    ASM_ASSERT

    printf("ASM\n");
    printf("GET LABELS 1\n");
    get_labels(words, labels, list);
    printf("GET FUNCKS\n");
    get_funcs(words, funcs, list);
    printf("HANDLE CMDS\n");
    size_t dig_amt = handle_cmds(words, labels, funcs, stk_code, list);    
    printf("FILL LABELS\n");
    fill_labels(words, labels, stk_code, list);
    printf("BIN CODE\n");
    int* bin_code = (int*)calloc(dig_amt, sizeof(int));
    
    for (size_t i = 0; i < dig_amt; i++)
        bin_code[i] = stk_code->data[i];

    /*printf("ARRAY\n");
    for (size_t i = 0; i < dig_amt; i++)
        printf("%d ", bin_code[i]);
    printf("\n");*/

    fill_bin_file(BIN_FILE_NAME, dig_amt, bin_code, list);

    free(bin_code);
}

//-------------------LABELS------------------------------

//----------------make and other----------------------------

LabelParameters* ctor_labels(ErrList *const list)
{
    assert(list);

    LabelParameters* labels = (LabelParameters*)calloc(LABELS_AMT, sizeof(LabelParameters));
    ALLOCATION_CHECK_PTR(labels)

    for (size_t i = 0; i < LABELS_AMT; i++)
    {
        labels[i].cmd_target = ERROR_VALUE_SIZE_T;
        labels[i].arg_target = ERROR_VALUE_SIZE_T;
        //labels[i].len = ERROR_VALUE_SIZE_T;
    }

    return labels;
}

void dtor_labels(LabelParameters *const labels)
{
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

    size_t lab_num = 0;

    while(labels[lab_num].cmd_target != ERROR_VALUE_SIZE_T)
    {
        int cmp_res = strncmp(word, labels[lab_num].start_word, len);

        if (cmp_res == 0)
            return lab_num;
    }

    return lab_num;
}

//--------------in code-------------------------

void get_labels(Word *const words, LabelParameters *const labels, ErrList *const list)
{
    //заполняет массив лэйблов, делает в словах пометки насчет лэйблов
    assert(words);
    assert(labels);
    assert(list);

    size_t word = 0;
    bool is_label = false;
    size_t label = 0;

    while (words[word].len != ERROR_VALUE_SIZE_T)
    {
        printf("WORD\n", word);
        printf("len %d\ndata %.5s\ntype %d\n------\n\n", words[word].len, words[word].word_start, words[word].type);
        is_label = find_label_mark(words[word].word_start, words[word].len);

        if(is_label)
        {
            printf("IS LABEL\n");
            label = find_label(labels, words[word].word_start, words[word].len);  //с этой штукой стоит еще разобраться. Скорее всего и так работать будет, но можно проще сделать
            printf("LABEL %d\n", label);
            if (words[word].type == CMD)
            {
                words[word].type = LABEL_CMD;
                labels[label].start_word = words[word].word_start;
                labels[label].len = words[word].len;
            }
            else
                words[word].type = LABEL_ARG;
        }

        is_label = false;
        word++;
    }

    printf("LABELS\n");

    for (int i  = 0; i < LABELS_AMT; i++)
        printf("text %.5s, len %d\n", labels[i].start_word, labels[i].len);
    printf("LABELS END\n\n");
}

void fill_labels(Word *const words, LabelParameters *const labels, Stack *const stk_code, ErrList *const list)
{
    //используется в конце, чтобы заполнить нужными адресами ассемблерный код
    assert(words);
    assert(labels);
    assert(stk_code);
    assert(list);

    size_t label = 0;
    stack_element_t* data = stk_code->data;

    while (labels[label].len != 0)
    {
        printf("LABEL arg_target %d, cmd_target %d, text %.5s, len %d\n", labels[label].arg_target, labels[label].cmd_target, labels[label].start_word, labels[label].len);
        data[labels[label].arg_target] = labels[label].cmd_target;
        label++;
    }
}

//----------------------FUNCS----------------------//да, очень похоже на лэйблы, но функция структура другая, поэтому так вот

//----------------make and other----------------------------

FuncParameters* ctor_funcs(ErrList *const list)
{
    assert(list);

    FuncParameters* funcs = (FuncParameters*)calloc(FUNCS_AMT, sizeof(FuncParameters));
    ALLOCATION_CHECK_PTR(funcs)

    for (size_t i = 0; i < FUNCS_AMT; i++)
    {
        funcs[i].ret_array = (int*)calloc(RET_AMT, sizeof(int));
        for (size_t j = 0; j < RET_AMT; j++)
            funcs[i].ret_array[j] = ERROR_VALUE_INT;
        
        funcs[i].len = ERROR_VALUE_SIZE_T;
        funcs[i].call_target = ERROR_VALUE_SIZE_T;
        funcs[i].ret_target = ERROR_VALUE_SIZE_T;
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

    while(funcs[func_num].call_target != ERROR_VALUE_SIZE_T)
    {
        int cmp_res = strncmp(word, funcs[func_num].start_word, len);

        if (cmp_res == 0)
            return func_num;
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

    while (words[word].len != ERROR_VALUE_SIZE_T)  //поискал по call-ам
    {
        int cmp_res = strncmp(words[word].word_start, CALL_STR, strlen(CALL_STR));

        if(cmp_res == 0)
        {
            word++;
            func_num = find_func(funcs, words[word].word_start, words[word].len);  //с этой штукой стоит еще разобраться. Скорее всего и так работать будет, но можно проще сделать
            
            funcs[func_num].start_word = words[word].word_start;
            funcs[func_num].len = words[word].len;
            func_amt++;
        }
        word++;
    }

    for (size_t num = 0; num < func_amt; num++)  //сопоставил ret-ы
    {
        while (words[word].len != ERROR_VALUE_SIZE_T) 
        {
            int cmp_res = strncmp(words[word].word_start, funcs[num].start_word, funcs[num].len);

            if(cmp_res == 0 && words[word].type == CMD)
            {
                cmp_res = ERROR_VALUE_INT;

                while (cmp_res != 0)
                {
                    word++;
                    cmp_res = strncmp(words[word].word_start, RET_STR, strlen(RET_STR));
                }

                funcs[num].ret_word = word;  //указывает, какое слово по счету в коде какой функции соответствует
            }
            word++;
        }
    }
    
}

void fill_func_ret(size_t word_num, FuncParameters func, ErrList *const list)
{
    assert(list);

    for (size_t i = 0; i < RET_AMT; i++)
    {
        if (func.ret_array[i] == ERROR_VALUE_INT);
        {
            func.ret_array[i] = word_num;
            break;
        }
    }
}

void fill_func_args(size_t *const word_num, Word *const words, FuncParameters func, ErrList *const list)
{
    //заполняет массив аргументов и количество аргументов
    assert(words);
    assert(word_num);
    assert(list);

    size_t var_num = 0;
    
    do 
    {
        func.vars[var_num] = atoi(words[*word_num].word_start);
        var_num++;
        (*word_num)++;
    }while (words[*word_num].str_num == words[(*word_num) + 1].str_num);
    //то есть остановится тогда, когда будет на последнем аргументе

    func.var_amt = var_num;
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

    //int a = 0;
    //scanf("%d", &a);

    while (words[word].len != ERROR_VALUE_SIZE_T)
    {
        //printf("word %d\n", word);
        //printf("len %d\ndata %.5s\ntype %d\n------\n\n", words[word].len, words[word].word_start, words[word].type);

        if (words[word].type == LABEL_CMD)
        {
            label = find_label(labels, words[word].word_start, words[word].len);
            labels[label].cmd_target = dig_amt - 1; //учет того, что начинается с 0
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
                funcs[func_num].call_target = dig_amt + 1;
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
            stk_push(stk_code, func_num, list);
            fill_func_ret(dig_amt, funcs[func_num], list); //заполняет массив ретов
            fill_func_args(&word, words, funcs[func_num], list);

            word++;
            continue;
        }
        else if (bunch_of_commands[cmd_num].cmd_num == RET_A)
        {
            stk_push(stk_code, RET_A, list);
            size_t func = find_func_by_ret_num(word, funcs, list);
            stk_push(stk_code, func, list);

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
    //число
    if (isdigit(word.word_start[0]))
    {
        arg = atoi(word.word_start); 
        stk_push(stk_code, ARG_NUM, list);
        stk_push(stk_code, arg, list);
    }
    //оп память
    else if(word.word_start[0] == RAM_MARK)
    {
        arg = atoi(word.word_start + 1);
        stk_push(stk_code, ARG_RAM, list);
        stk_push(stk_code, arg, list);
    }
    //лэйбл
    else if(word.type == LABEL_ARG)
    {
        stk_push(stk_code, ARG_LAB, list);
        stk_push(stk_code, ERROR_VALUE_INT, list);
        size_t label = find_label(labels, word.word_start, word.len);
        labels[label].arg_target = dig_num - 1;  //учет того, что счет начинается с 0
    }
    //регистр
    /*else
    {
        size_t reg_num = find_reg(word.word_start, word.len);
    }*/
}

//-------------------------BIN_FILE--------------------

void fill_bin_file(const char* const  input_file_name, size_t size, const int* const input_file_data, ErrList *const list)
{
    assert(input_file_name);
    assert(input_file_data);
    assert(list);
    
    FILE* output_file;
    printf("FILE\n");
    output_file = fopen(input_file_name, "w");
    printf("HERE\n");
    FILE_CHECK(output_file)
    
    for (size_t i = 0; i < size; i++)
        fprintf(output_file, "%d ", input_file_data[i]);
        
    int close_res = fclose(output_file);
    CLOSE_CHECK
}