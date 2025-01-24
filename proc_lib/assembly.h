#ifndef _ASM_H_
#define _ASM_H_

#include <stdint.h>
#include "stk.h"

static const char* BIN_FILE_NAME = "bin_code.txt";

LabelParameters* ctor_labels(ErrList *const list);
void dtor_labels(LabelParameters *const labels);

void assembly(Word *const words, LabelParameters *const labels, Stack *const stk_code, ErrList *const list);

#define ASM_ASSERT  do                      \
                    {                       \
                        assert(words);      \
                        assert(labels);     \
                        assert(funcs);      \
                        assert(stk_code);   \
                        assert(list);       \
                    } while(0);

#endif //_ASM_H_