#include <stdio.h>

#include "proc.h"

static int get_arg(Proc *const proc, size_t *const ip);
static void give_arg(Proc *const proc, Stack *const prog, size_t *const ip, ErrList *const list);

static ResultOfComparing comparing(int first_el, int sec_el);
static void ja(Proc* proc, Stack* prog, size_t *const ip, ErrList *const list);
static void jae(Proc* proc, Stack* prog, size_t *const ip, ErrList *const list);
static void jb(Proc* proc, Stack* prog, size_t *const ip, ErrList *const list);
static void jbe(Proc* proc, Stack* prog, size_t *const ip, ErrList *const list);
static void je(Proc* proc, Stack* prog, size_t *const ip, ErrList *const list);
static void jne(Proc* proc, Stack* prog, size_t *const ip, ErrList *const list);

static void get_call(Proc *const proc, Stack *const prog, Stack *const stk, size_t *const ip, ErrList *const list);
static void proc_code_part(Proc *const proc, Stack *const prog, Stack *const stk, size_t *const ip, ErrList *const list);
static void get_ret (Proc *const proc, Stack *const stk, size_t *const ip, ErrList *const list);

void proc_ctor(Proc *const proc, ErrList *const list)
{
    assert(proc);
    assert(list);

    int* code = (int*)calloc(MAX_FILE_SIZE, sizeof(int));
    ALLOCATION_CHECK_VOID(code)

    int* RAM = (int*)calloc(RAM_AMT, sizeof(int));
    ALLOCATION_CHECK_VOID(RAM)


    for (size_t i = 0; i < RAM_AMT; i++)
        RAM[i] = POISON;

    proc->code = code;
    proc->RAM = RAM;
}

void proc_dtor(Proc *const proc)
{
    free(proc->code);
    free(proc->RAM);
}


void proc_code(Proc *const proc, Stack *const prog, Stack *const stk, ErrList *const list)
{
    assert(proc);
    assert(list);

    size_t ip = 0;
    
    size_t size = proc->size;

    int* code = proc->code;

    int first_el = 0;
    int sec_el   = 0;
    int arg      = 0;

    while(ip < size)
    {

        switch(code[ip])
        {
            case PUSH_A:
                arg = get_arg(proc, &ip);
                stk_push(prog, arg, list);
                break;

            case ADD_A:
                GET_TWO_ARGS
                stk_push(prog, first_el + sec_el, list);
                break;

            case SUB_A:
                GET_TWO_ARGS
                stk_push(prog, first_el - sec_el, list);
                break;

            case MUL_A:
                GET_TWO_ARGS
                stk_push(prog, first_el * sec_el, list);
                break;

            case DIV_A:
                GET_TWO_ARGS
                stk_push(prog, first_el / sec_el, list);
                break;
            
            case JA_A:
                ja(proc, prog, &ip, list);
                break;

            case JAE_A:
                jae(proc, prog, &ip, list);
                break;

            case JB_A:
                jb(proc, prog, &ip, list);
                break;

            case JBE_A:
                jbe(proc, prog, &ip, list);
                break;

            case JE_A:
                je(proc, prog, &ip, list);
                break;

            case JNE_A:
                jne(proc, prog, &ip, list);
                break;

            case JMP_A:
                arg = get_arg(proc, &ip);
                ip = arg;
                break;

            case POP_A:
                give_arg(proc, prog, &ip, list);
                break;

            case SQRT_A:
                stk_pop(prog, &arg, list);
                stk_push(prog, sqrt(arg), list);
                break;

            case CALL_A:
                get_call(proc, prog, stk, &ip, list);
                break;

            case HLT_A:
                return;
                break;

            case OUT_A:
                stk_pop(prog, &arg, list);
                printf("!! %d !!\n", arg);
                break;
            //    break;

            default:
                printf("problem\n");
                break;
        }   
        ip++;
        
        /*printf("RAM\n");
        for (size_t i = 0; i < 10; i++)
            printf("%d ", proc->RAM[i]);
        printf("\nRAM END\n\n");*/

        //int a = 0;
        //scanf("%d", &a);
    }
}

void proc_code_part(Proc *const proc, Stack *const prog, Stack *const stk, size_t *const ip, ErrList *const list)
{
    assert(proc);
    assert(list);

    //printf("GET PART\n");

    size_t size = proc->size;

    int* code = proc->code;

    int first_el = 0;
    int sec_el   = 0;
    int arg      = 0;

    while(*ip < size)
    {
        //printf("ip %d\n", *ip);
        //printf("CODE %d %d %d\n", code[*ip], code[*ip + 1], code[*ip + 2]);
        switch(code[*ip])
        {
            case PUSH_A:
                //printf("push\n");
                arg = get_arg(proc, ip);
                //printf("ARG %d\n\n", arg);
                stk_push(prog, arg, list);
                break;

            case ADD_A:
                //printf("add\n");
                GET_TWO_ARGS
                stk_push(prog, first_el + sec_el, list);
                break;

            case SUB_A:
                //printf("sub\n");
                GET_TWO_ARGS
                stk_push(prog, first_el - sec_el, list);
                break;

            case MUL_A:
                //printf("mul\n");
                GET_TWO_ARGS
                stk_push(prog, first_el * sec_el, list);
                break;

            case DIV_A:
                //printf("div\n");
                GET_TWO_ARGS
                stk_push(prog, first_el / sec_el, list);
                break;
            
            case JA_A:
                //printf("ja\n");
                ja(proc, prog, ip, list);
                break;

            case JAE_A:
                jae(proc, prog, ip, list);
                break;

            case JB_A:
                jb(proc, prog, ip, list);
                break;

            case JBE_A:
                jbe(proc, prog, ip, list);
                break;

            case JE_A:
                je(proc, prog, ip, list);
                break;

            case JNE_A:
                //printf("jne\n");
                jne(proc, prog, ip, list);
                break;

            case JMP_A:
                arg = get_arg(proc, ip);
                *ip = arg;
                break;

            case POP_A:
                //printf("pop\n");
                give_arg(proc, prog, ip, list);
                break;

            case CALL_A:
                //printf("CALL\n");
                get_call(proc, prog, stk, ip, list);
                break;

            case HLT_A:
                //printf("hlt\n");
                return;
                break;

            case RET_A:
                //printf("RET\n");
                get_ret (proc, stk, ip, list);
                return; //обязательно! именно эта часть и вернет из функции get call
                break;

            default:
                printf("problem\n");
                break;
        }   
        (*ip)++;
        
        /*printf("RAM\n");
        for (size_t i = 0; i < 10; i++)
            printf("%d ", proc->RAM[i]);
        printf("\nRAM END\n\n");
        int a = 0;
        scanf("%d", &a);*/
    }
}

void get_call(Proc *const proc, Stack *const prog, Stack *const stk, size_t *const ip, ErrList *const list)
{
    assert(proc);
    assert(prog);
    assert(list);
    assert(ip);

    int* code = proc->code;
    
    (*ip)++;
    stk_push(stk, *ip, list);
    (*ip) = code[*ip];

    proc_code_part(proc, prog, stk, ip, list);
}

void get_ret (Proc *const proc, Stack *const stk, size_t *const ip, ErrList *const list)
{
    assert(proc);
    assert(ip);
    assert(list);

    int* code = proc->code;
    int ret_ip = 0;
    stk_pop(stk, &ret_ip, list);

    *ip = ret_ip;
}

int get_arg(Proc *const proc, size_t *const ip)
{
    assert(proc);
    assert(ip);

    (*ip)++;
    int arg = 0;
    int* prog = proc->code;
    int* RAM = proc->RAM;

    /*ARG_NUM, 
    ARG_LAB,
    ARG_RAM,
    ARG_REG*/
    //printf("TYPE %d\n\n", prog[*ip]);

    if (prog[*ip] == ARG_NUM || prog[*ip] == ARG_LAB)
    {
        //printf("NUM\n");
        (*ip)++;
        return prog[*ip];
    }
    else if (prog[*ip] == ARG_RAM)
    {
        //printf("RAM\n");
        (*ip)++;
        //printf("PLACE %d\n\n", prog[*ip]);
        arg = RAM[prog[*ip]];
        //printf("ARG %d\n\n", arg);
    }
    /*else if (prog[ip] == ARG_REG)
    {

    }*/

   return arg;
}

void ja(Proc* proc, Stack* prog, size_t *const ip, ErrList *const list)
{
    assert(proc);
    assert(prog);
    assert(ip);
    assert(list);

    int* code = proc->code;
    *ip += 2;
    size_t new_ip = (size_t)code[*ip];
    

    stack_element_t first_el = 0;
    stack_element_t sec_el = 0;

    stk_pop(prog, &sec_el, list);
    stk_pop(prog, &first_el, list);

    bool do_jump = false;

    ResultOfComparing res = comparing(first_el, sec_el);
    if (res == GREATER_RES)
    {
        //printf("0 IS GREATER THAN DISCR\n");
        do_jump = true;
    }
        
    if(do_jump)
    {
        //printf("NEW IP %d\n", new_ip);
        *ip = new_ip;
    }
        
}

void jae(Proc* proc, Stack* prog, size_t *const ip, ErrList *const list)
{
    assert(proc);
    assert(prog);
    assert(ip);
    assert(list);

    int* code = proc->code;
    *ip += 2;
    size_t new_ip = (size_t)code[*ip];

    stack_element_t first_el = 0;
    stack_element_t sec_el = 0;

    stk_pop(prog, &sec_el, list);
    stk_pop(prog, &first_el, list);

    bool do_jump = false;

    ResultOfComparing res = comparing(first_el, sec_el);
    if (res == GREATER_RES || res == EQUAL_RES)
        do_jump = true;

    if(do_jump)
        *ip = new_ip;
}

void jb(Proc* proc, Stack* prog, size_t *const ip, ErrList *const list)
{
    assert(proc);
    assert(prog);
    assert(ip);
    assert(list);

    int* code = proc->code;
    *ip += 2;
    size_t new_ip = (size_t)code[*ip];

    stack_element_t first_el = 0;
    stack_element_t sec_el = 0;

    stk_pop(prog, &sec_el, list);
    stk_pop(prog, &first_el, list);

    bool do_jump = false;

    ResultOfComparing res = comparing(first_el, sec_el);
    if (res == LESS_RES)
        do_jump = true;

    if(do_jump)
        *ip = new_ip;
}

void jbe(Proc* proc, Stack* prog, size_t *const ip, ErrList *const list)
{
    assert(proc);
    assert(prog);
    assert(ip);
    assert(list);

    int* code = proc->code;
    *ip += 2;
    size_t new_ip = (size_t)code[*ip];

    stack_element_t first_el = 0;
    stack_element_t sec_el = 0;

    stk_pop(prog, &sec_el, list);
    stk_pop(prog, &first_el, list);

    bool do_jump = false;

    ResultOfComparing res = comparing(first_el, sec_el);
    if (res == LESS_RES || res == EQUAL_RES)
        do_jump = true;

    if(do_jump)
        *ip = new_ip;
}

void je(Proc* proc, Stack* prog, size_t *const ip, ErrList *const list)
{
    assert(proc);
    assert(prog);
    assert(ip);
    assert(list);

    int* code = proc->code;
    *ip += 2;
    size_t new_ip = (size_t)code[*ip];

    stack_element_t first_el = 0;
    stack_element_t sec_el = 0;

    stk_pop(prog, &sec_el, list);
    stk_pop(prog, &first_el, list);

    bool do_jump = false;

    ResultOfComparing res = comparing(first_el, sec_el);
    if (res == EQUAL_RES)
        do_jump = true;

    if(do_jump)
        *ip = new_ip;
}

void jne(Proc* proc, Stack* prog, size_t *const ip, ErrList *const list)
{
    assert(proc);
    assert(prog);
    assert(ip);
    assert(list);

    int* code = proc->code;
    *ip += 2;
    size_t new_ip = (size_t)code[*ip];

    stack_element_t first_el = 0;
    stack_element_t sec_el = 0;

    stk_pop(prog, &sec_el, list);
    stk_pop(prog, &first_el, list);

    bool do_jump = false;

    ResultOfComparing res = comparing(first_el, sec_el);
    if (res != EQUAL_RES)
    {
        //printf ("THEY ARE NOT EQUAL\n");
        do_jump = true;
    }
        

    if(do_jump)
    {
        //printf("NEW IP %d\n", new_ip);
        *ip = new_ip;
    }
        
}

ResultOfComparing comparing(int first_el, int sec_el)
{
    if (first_el > sec_el)
        return GREATER_RES;

    else if (first_el == sec_el)
        return EQUAL_RES;

    else 
        return LESS_RES;
}

void give_arg(Proc *const proc, Stack *const prog, size_t *const ip, ErrList *const list)
{
    assert(proc);
    assert(prog);
    assert(ip);
    assert(list);

    (*ip)++;
    int arg = 0;
    int* code = proc->code;
    int* RAM = proc->RAM;

    /*ARG_NUM, 
    ARG_LAB,
    ARG_RAM,
    ARG_REG*/

    if (code[*ip] == ARG_NUM)  //при ассемблировании тут будет фиктивное значение
        (*ip)++;

    else if (code[*ip] == ARG_RAM)
    {
        //printf("RAM case\n\n");
        stk_pop(prog, &arg, list);
        //printf("ARG RAM %d\n\n", arg);
        (*ip)++;
        //printf("PLACE %d\n\n", code[*ip]);
        RAM[code[*ip]] = arg;

        //printf("RAM\n\n");
        //for (int i = 0; i < 10; i++)
        //    printf("%d ", RAM[i]);
        //printf("RAM END\n\n");
    }
    /*else if (code[ip] == ARG_REG)
    {

    }*/
}