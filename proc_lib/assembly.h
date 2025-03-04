#ifndef _ASM_H_
#define _ASM_H_

#include <stdint.h>
#include "stk.h"

LabelParameters* ctor_labels(ErrList *const list);
void dtor_labels(LabelParameters *const labels);

FuncParameters* ctor_funcs(ErrList *const list);
void dtor_funcs(FuncParameters *const funcs);

void assembly(Word *const words, LabelParameters *const labels, FuncParameters *const funcs, Stack *const stk_code, ErrList *const list);


#define ASM_ASSERT  do                      \
                    {                       \
                        assert(words);      \
                        assert(labels);     \
                        assert(funcs);      \
                        assert(stk_code);   \
                        assert(list);       \
                    } while(0);

#endif //_ASM_H_