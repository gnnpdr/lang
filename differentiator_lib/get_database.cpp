#include <stdio.h>

#include "get_database.h"

static void count_file_size(const char *const name, size_t* size, Err_param *const error);


void input_ctor (Input *const base_text, Err_param *const error)
{
    assert(base_text);
    assert(error);

    char* name = (char*)calloc(MAX_STR_LEN, sizeof(char));
    ALLOCATION_CHECK(name)

    char* text = (char*)calloc(MAX_FILE_SIZE, sizeof(char));
    ALLOCATION_CHECK(name)

    base_text->name = name;
    base_text->text = text;
}

void get_database_name(Input *const base_text, char **const argv, Err_param *const error)
{
    assert(base_text);
    assert(argv);
    assert(error);

    char* name = base_text->name;

    strncpy(name, argv[1], strlen(argv[1]) + 1);
    CPY_CHECK(name)
        
    base_text->name = name;
}

void get_database_text (Input *const base_text, Err_param *const error)
{
    assert(base_text);
    assert(error);

    char* base_file_name = base_text->name;
    FILE* input_file;

    input_file = fopen(base_file_name, "rb");
    FILE_CHECK(input_file)

    size_t size = 0;

    count_file_size(base_text->name, &size, error);
    RETURN_VOID

    char* text = base_text->text;

    size_t read_result = fread(text, sizeof(char), size, input_file);
    READ_CHECK
        
    int close_res = fclose(input_file);
    CLOSE_CHECK

    base_text->text = text;
    base_text->size = size;
}

void count_file_size(const char *const name, size_t* size, Err_param *const error)
{
    assert(name);
    assert(size);

    struct stat file_info;

    STAT_CHECK(name)

    *size = file_info.st_size;
}

void input_dtor(Input* base_text)
{
    free(base_text->name);
    free(base_text->text);
}