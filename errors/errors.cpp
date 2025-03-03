#include <stdio.h>

#include "errors.h"

static void fill_error(Err_param *const error, LOCATION_DEF, Errors err);

void fill_error(Err_param *const error, LOCATION_DEF, Errors err)
{
    assert(error);
    assert(file);
    assert(func);

    error->err_num = err;
    error->file = file;
    error->func = func;
    error->line = line;
}

void error_list_ctor(ErrList *const list)
{
    assert(list);

    Err_param* err_list = (Err_param*)calloc(ERRORS_AMT, sizeof(Err_param));
    list->list = err_list;
    list->head = 0;
}

void list_push(ErrList *const list, LOCATION_DEF, Errors err)
{
    assert(list);
    assert(file);
    assert(func);
    assert(err);

    fill_error(list->list + list->head, file, func, line, err);
    list->head++;
}

void error_list_dtor(ErrList *const list)
{
    free(list->list);
}
