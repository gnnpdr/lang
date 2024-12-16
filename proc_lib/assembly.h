#ifndef _ASM_H_
#define _ASM_H_

#include <stdint.h>
#include "stk.h"

static const char* BIN_FILE_NAME = "bin_code";

LabelParameters* ctor_labels(ErrList *const list);
void dtor_labels(LabelParameters *const labels);

void assembly(Word *const words, LabelParameters *const labels, Stack *const stk_code, ErrList *const list);


#endif //_ASM_H_