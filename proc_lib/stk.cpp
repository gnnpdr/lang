#include <stdio.h>

#include "stk.h"

static void change_capacity(Stack *const stk, ErrList *const list);

//---------------------CTOR-DTOR-----------------------------------------------------------------------

void stk_ctor(Stack *const stk, ErrList *const list)
{
    //printf("STACK CTOR\n");
    STK_ASSERT

    stk->size = 0;
    stk->capacity = START_STK_SIZE;

    size_t capacity = stk->capacity;

    stack_element_t* data = (stack_element_t*)calloc(capacity, sizeof(stack_element_t));
    ALLOCATION_CHECK_VOID(data)
    
    for (size_t i = 0; i < capacity; i++)
        data[i] = POISON;

    stk->data = data;
}

void stk_dtor(Stack *const stk)
{
    free(stk->data);
}

//------------FUNCTIONS-------------------------------------------------------------------------------------------------


void stk_push(Stack *const stk, stack_element_t element, ErrList *const list)
{
    //printf("STK PUSH\n");
    //printf("ELEM %d\n", element);
    STK_ASSERT

    size_t size     = stk->size;
    size_t capacity = stk->capacity;

    if (size >= capacity)
    {
        change_capacity(stk, list);
        RETURN_VOID
    }
    
    stack_element_t* data = stk->data;
    data[size] = element;
    
    size++;
    stk->size = size;

    /*printf("--------------------------------\nSTACK PUSH\n");
    printf("\n\n");
    for (size_t i = 0; i < stk->capacity; i++)
            printf("%d ", stk->data[i]);
    printf("\n");*/
}


void change_capacity(Stack *const stk, ErrList *const list)
{
    STK_ASSERT

    size_t size = stk->size;
    size_t capacity = stk->capacity;
    stack_element_t* data = stk->data;

    size_t new_capacity = 0;

    if (size >= capacity)
        new_capacity = capacity * DELTA;
    else
        new_capacity = capacity / DELTA;

    data = (stack_element_t*)realloc(data, new_capacity * sizeof(stack_element_t));
    ALLOCATION_CHECK_VOID(data)
    
    for (size_t i = 0; i <= capacity; i++)
        data[capacity + i] = POISON;

    stk->capacity = new_capacity;
    stk->data = data; 
}

void stk_pop(Stack *const stk, stack_element_t* elem, ErrList *const list)
{
    STK_ASSERT
    assert(elem);

    size_t size = stk->size;
    size_t capacity = stk->capacity;

    if (size == (capacity / DOUBLE_DELTA) && size > MIN_STK_SIZE)
    {
        change_capacity(stk, list);
        RETURN_VOID
    }

    stack_element_t* data = stk->data;
    
    size--; 
    *elem = data[size];
    data[size] = POISON;
    
    stk->size = size;
    stk->data = data;

    /*printf("--------------------------------\nSTACK POP\n");
    printf("\n\n");
    for (size_t i = 0; i < stk->capacity; i++)
            printf("%d ", stk->data[i]);
    printf("\n");*/
}
