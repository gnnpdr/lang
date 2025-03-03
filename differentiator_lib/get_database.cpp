#include <stdio.h>

#include "get_database.h"


static void get_database_name(Input *const base_text, char *const input_name, ErrList *const list);
static void get_database_text (Input *const base_text, ErrList *const list);

void input_ctor (Input *const base_text, ErrList *const list)
{
    assert(base_text);
    assert(list);

    char* name = (char*)calloc(MAX_STR_LEN, sizeof(char));
    ALLOCATION_CHECK_VOID(name)

    char* text = (char*)calloc(MAX_FILE_SIZE, sizeof(char));
    ALLOCATION_CHECK_VOID(text)

    base_text->name = name;
    printf("NAME ADDRESS %p\nTEXT ADDRESS %p\n", name, text);
    base_text->text = text;

    printf("IN INPUT CTOR\n");
    int a = 0;
    scanf("%d", &a);
}

void input_dtor(Input* base_text)
{
    //printf("FREE NAME\n");
    free(base_text->name);
    //printf("FREE TEXT\n");
    free(base_text->text);
    
}

void get_text(Input *const base_text, char *const input_name, ErrList *const list)
{
    assert(base_text);
    assert(input_name);
    assert(list);

    get_database_name(base_text, input_name, list);
	RETURN_VOID

    get_database_text(base_text, list);
	RETURN_VOID

    //printf("----------\nNAME\n%s\n\nTEXT\n%s\n----------------\n", base_text->name, base_text->text);
}

/*void get_text_wname (Input *const text, const char *const name, ErrList *const list)
{
    assert(text);
    assert(list);

    strncpy(text->name, name, MAX_STR_LEN);
    //printf("6791\n");
    CPY_CHECK(text->name)
    //printf("!!!ooo\n");
    get_database_text(text, list);
    RETURN_VOID
}*/

void get_database_name(Input *const base_text, char *const input_name, ErrList *const list)
{
    assert(base_text);
    assert(input_name);
    assert(list);

    char* name = base_text->name;

    strncpy(name, input_name, strlen(input_name) + 1);
    CPY_CHECK(name)
        
    base_text->name = name;
}

void get_database_text (Input *const base_text, ErrList *const list)
{
    assert(base_text);
    assert(list);

    char* base_file_name = base_text->name;
    FILE* input_file;

    input_file = fopen(base_file_name, "rb");
    FILE_CHECK(input_file)
    size_t size = 0;

    count_file_size(base_text->name, &size, list);
    printf("FILE SIZE %d\n", size);
    RETURN_VOID
    char* text = base_text->text;

    size_t read_result = fread(text, sizeof(char), size, input_file);
    READ_CHECK
    strncpy(base_text->text, text, size);

    int close_res = fclose(input_file);
    CLOSE_CHECK
    
    base_text->size = size;

    printf("IN GET DATABASE TEXT\n");
    int a = 0;
    scanf("%d", &a); 
}

void count_file_size(const char *const name, size_t* size, ErrList *const list)
{
    assert(name);
    assert(size);

    struct stat file_info;
    STAT_CHECK(name)

    *size = file_info.st_size;
}