#ifndef _PROC_H_
#define _PROC_H_

#include "assembly.h"

#include "stk.h"

static const size_t RAM_AMT       = 50;

enum ResultOfComparing
{
    LESS_RES = 0,
    EQUAL_RES = 1,
    GREATER_RES = 2,
};

struct Proc
{
    size_t size;

    int* code;

    int* RAM;
};

void proc_ctor(Proc *const proc, ErrList *const list);
void proc_dtor(Proc *const proc);

void proc_code(Proc *const proc, Stack *const prog, Stack *const stk, ErrList *const list);

#define GET_TWO_ARGS    do                                  \
                        {                                   \
                            /*sec_el = */stk_pop(prog, &sec_el, list);    \
                            /*first_el = */stk_pop(prog, &first_el, list);  \
                        } while(0);

#define JUMP_INFO       assert(stk != nullptr);                     \
                        assert(processor != nullptr);               \
                        assert(err != nullptr);                     \
                                                                    \
                        size_t ip = processor->ip;                  \
                        int* program = processor->new_file_buf;     \
                        stack_element_t first_el = 0;               \
                        stack_element_t sec_el = 0;                 \
                                                                    \
                        stk_pop(stk, &sec_el);                      \
                        stk_pop(stk, &first_el);   

                        
#endif //_PROC_H_