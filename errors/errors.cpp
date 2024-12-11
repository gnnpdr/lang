#include <stdio.h>

#include "errors.h"

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