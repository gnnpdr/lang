#include <stdio.h>

#include "assembly.h"

static void get_labels(Word *const words, LabelParameters *const labels, ErrList *const list);
static bool find_label_mark(char *const word, size_t len);
static size_t find_label (LabelParameters *const labels, char *const word, size_t len);
static size_t handle_cmds(Word *const words, LabelParameters *const labels, Stack *const stk_code, ErrList *const list);
static void handle_args(Word word, LabelParameters *const labels, Stack *const stk_code, ErrList *const list);
static void fill_bin_file(const char* const  input_file_name, size_t size, const int* const input_file_data, ErrList *const list);

void assembly(Word *const words, LabelParameters *const labels, Stack *const stk_code, ErrList *const list)
{
    assert(words);
    assert(labels);
    assert(stk_code);
    assert(list);

    get_labels(words, labels, list);
    //get_labels(words, labels, list);  //это еще понадобится
    //printf("HERE\n");
    size_t dig_amt = handle_cmds(words, labels, stk_code, list);

    int* bin_code = (int*)calloc(dig_amt, sizeof(int));
    
    for (size_t i = 0; i < dig_amt; i++)
        bin_code[i] = stk_code->data[i];

    fill_bin_file(BIN_FILE_NAME, dig_amt, bin_code, list);

    free(bin_code);
}

LabelParameters* ctor_labels(ErrList *const list)
{
    assert(list);

    LabelParameters* labels = (LabelParameters*)calloc(LABELS_AMT, sizeof(LabelParameters));
    ALLOCATION_CHECK_PTR(labels)

    for (size_t i = 0; i < LABELS_AMT; i++)
        labels[i].target = ERROR_VALUE_INT;  //надо сделать в структуре сайз т

    return labels;
}

void dtor_labels(LabelParameters *const labels)
{
    free(labels);
}

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
        //printf("here\n");
        //for (int i = 0; i < 20; i++)
        //    printf("text %.10s\nlen %d\nstr %d\ntype %d\n----------\n", words[i].word_start, words[i].len, words[i].str_num, words[i].type);
        //printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        is_label = find_label_mark(words[word].word_start, words[word].len);

        if(is_label)
        {
            label = find_label(labels, words[word].word_start, words[word].len);  //с этой штукой стоит еще разобраться. Скорее всего и так работать будет, но можно проще сделать
            //printf("is label\n");
            if (words[word].type == CMD)
            {
                words[word].type = LABEL_CMD;
                labels[label].start_word = words[word].word_start;
                labels[label].len = words[word].len;
                labels[label].target = words[word].str_num;
            }
            else
                words[word].type = LABEL_ARG;
        }

        is_label = false;
        word++;
    }
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

    while(labels[lab_num].target != ERROR_VALUE_INT)
    {
        int cmp_res = strncmp(word, labels[lab_num].start_word, len);

        if (cmp_res == 0)
            return lab_num;
    }

    return lab_num;
}

size_t handle_cmds(Word *const words, LabelParameters *const labels, Stack *const stk_code, ErrList *const list)
{
    assert(words);
    assert(labels);
    assert(stk_code);
    assert(list);

    size_t word = 0;
    bool is_label = false;
    size_t label = 0;
    size_t dig_amt = 0;

    while (words[word].len != ERROR_VALUE_SIZE_T)
    {
        if (words[word].type == LABEL_CMD)
        {
            word++;
            continue;
        }
            
        size_t cmd_num = ERROR_VALUE_SIZE_T;
        printf("cmd %.10s\nlen %d\n", words[word].word_start, words[word].len);
        for (size_t cmd = 0; cmd < CMD_AMT; cmd++)  //можно переделать в вайл, там будет красивее
        {
            int cmp_res = strncmp(words[word].word_start, bunch_of_commands[cmd].cmd_str, words[word].len);

            if (cmp_res == 0)
                cmd_num = cmd;
        }

        if(cmd_num == ERROR_VALUE_SIZE_T)
        {
            ERROR(SYN_ERROR)
            return ERROR_VALUE_SIZE_T;
        }
        dig_amt++;
        stk_push(stk_code, bunch_of_commands[cmd_num].cmd_num, list);
        printf("arg amt %d\n", bunch_of_commands[cmd_num].arg_amt);
        
        for (size_t arg = 0; arg < bunch_of_commands[cmd_num].arg_amt; arg++)
        {
            word++;
            dig_amt += 2;
            handle_args(words[word], labels, stk_code, list);
        }
          
        word++;
    }

    return dig_amt;
}

void handle_args(Word word, LabelParameters *const labels, Stack *const stk_code, ErrList *const list)
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
    //лэйбл
    else if(word.type == LABEL_ARG)
    {
        size_t lab_num = find_label(labels, word.word_start, word.len);
        stk_push(stk_code, ARG_LAB, list);
        stk_push(stk_code, labels[lab_num].target, list);
    }
    //оп память
    else if(word.word_start[0] == RAM_MARK)
    {
        arg = atoi(word.word_start + 1);
        stk_push(stk_code, ARG_RAM, list);
        stk_push(stk_code, arg, list);
    }
    // else - после всего этого будет выделяться тот случай - pop не требует аргументов вообще
    // для стандартизации закинем туда значение типа 0 или error, будто оно есть но фиктивное
    /* хорошо бы еще вставить проверку на то, что функция, вызвавшая этот аргумент - pop
    {
        
    }*/
    //регистр
    /*else      //потом расписать регистр, не до этого сейчас
    {
        size_t reg_num = find_reg(word.word_start, word.len);
    }*/
}

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
        
    //fwrite(input_file_data, sizeof(int), INPUT_FILE_SIZE, input_file); 
    
    int close_res = fclose(output_file);
    CLOSE_CHECK
}