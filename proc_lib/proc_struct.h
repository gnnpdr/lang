#ifndef _PROC_STRUCT_H_
#define _PROC_STRUCT_H_

//static const size_t RAM_SIZE = 100;

struct Proc
{
    size_t size;

    int* code;

    int* RAM;
};

enum ResultOfComparing
{
    LESS_RES = 0,
    EQUAL_RES = 1,
    GREATER_RES = 2,
};

#endif //_PROC_STRUCT_H_