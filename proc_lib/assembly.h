#ifndef _ASM_H_
#define _ASM_H_

#include <stdint.h>
#include "stk.h"

void assembly (Input *const asm_text, Labels *const labels, Stack* new_buf, Stack* functions, ErrList *const list);
void ctor_labels(Labels* labels, ErrList *const list);
void dtor_labels(Labels* labels);
void make_binary_file (Stack *const new_buf, ErrList *const list);

#endif //_ASM_H_