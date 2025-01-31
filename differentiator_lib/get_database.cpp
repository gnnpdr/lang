#include <stdio.h>

#include "get_database.h"


static void get_database_name(Input *const base_text, char **const argv, ErrList *const list);
static void get_database_text (Input *const base_text, ErrList *const list);

void input_ctor (Input *const base_text, ErrList *const list)
{
    assert(base_text);
    assert(list);

    char* name = (char*)calloc(MAX_STR_LEN, sizeof(char));
    ALLOCATION_CHECK_VOID(name)

    char* text = (char*)calloc(MAX_FILE_SIZE, sizeof(char));
    ALLOCATION_CHECK_VOID(name)

    base_text->name = name;
    base_text->text = text;
}

void input_dtor(Input* base_text)
{
    printf("FREE NAME\n");
    free(base_text->name);
    printf("FREE TEXT\n");
    free(base_text->text);
    printf("OHH HERE\n");
}

void get_text(Input *const base_text, char **const argv, ErrList *const list)
{
    assert(base_text);
    assert(argv);
    assert(list);

    get_database_name(base_text, argv, list);
	RETURN_VOID

    get_database_text(base_text, list);
	RETURN_VOID
}

void get_text_wname (Input *const text, const char *const name, ErrList *const list)
{
    assert(text);
    assert(list);

    strncpy(text->name, name, MAX_STR_LEN);
    //printf("6791\n");
    CPY_CHECK(text->name)
    //printf("!!!ooo\n");
    get_database_text(text, list);
    //printf("#44\n");
    RETURN_VOID
}

void get_database_name(Input *const base_text, char **const argv, ErrList *const list)
{
    assert(base_text);
    assert(argv);
    assert(list);

    char* name = base_text->name;

    strncpy(name, argv[1], strlen(argv[1]) + 1);
    CPY_CHECK(name)
        
    base_text->name = name;
}

void get_database_text (Input *const base_text, ErrList *const list)
{
    assert(base_text);
    assert(list);

    char* base_file_name = base_text->name;
    FILE* input_file;
    printf("CTG 1\n");

    input_file = fopen(base_file_name, "rb");
    FILE_CHECK(input_file)
    printf("CTG 2\n");
    size_t size = 0;

    count_file_size(base_text->name, &size, list);
    RETURN_VOID
    printf("CTG 2\n");
    char* text = base_text->text;

    size_t read_result = fread(text, sizeof(char), size, input_file);
    READ_CHECK
    
    printf("CTH 4\n");
    strncpy(base_text->text, text, size);
    printf("CTH 5\n");

    printf("CTG 3\n");
    int close_res = fclose(input_file);
    printf("CLOSE CHECK!!!\n");
    CLOSE_CHECK
    
    base_text->size = size;
}

void count_file_size(const char *const name, size_t* size, ErrList *const list)
{
    assert(name);
    assert(size);

    struct stat file_info;
    STAT_CHECK(name)

    *size = file_info.st_size;
}