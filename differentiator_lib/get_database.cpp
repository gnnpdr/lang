#include <stdio.h>

#include "get_database.h"

static void count_file_size(const char *const name, size_t* size, ErrList *const list);

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

    input_file = fopen(base_file_name, "rb");
    FILE_CHECK(input_file)

    size_t size = 0;

    count_file_size(base_text->name, &size, list);
    RETURN_VOID

    char* text = base_text->text;
    

    size_t read_result = fread(text, sizeof(char), size, input_file);
    READ_CHECK
    //printf("OH HELLO\n%s\n", text);
    //вот здесь он нормально вывелся
    int close_res = fclose(input_file);
    CLOSE_CHECK

    //base_text->text = text;
    strncpy(base_text->text, text, size);
    //printf("OH HELLO\n%s\n", base_text->text);
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

void input_dtor(Input* base_text)
{
    free(base_text->name);
    free(base_text->text);
}
