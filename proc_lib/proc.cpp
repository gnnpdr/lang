#include <stdio.h>

#include "proc.h"

static int get_arg (Processor* proc);
static int* get_arg_addr (Processor* proc);

static void ja (Processor* proc, Stack* stk, ErrList *const list);
static void jae (Processor* proc, Stack* stk, ErrList *const list);
static void je (Processor* proc, Stack* stk, ErrList *const list);
static void jne (Processor* proc, Stack* stk, ErrList *const list);
static void jb(Processor* proc, Stack* stk, ErrList *const list);
static void jbe(Processor* proc, Stack* stk, ErrList *const list);
static ResultOfComparing comparing(int first_el, int sec_el);

//static void print_regs (RegisterParameters* registers);

void proc_file (Stack* const stk, Processor* const proc, Stack* functions, ErrList *const list)
{
    assert(stk);
    assert(proc);
    assert(functions);
    assert(list);

    size_t size = proc->ncmd;

    size_t ret_cnt = 0;
    
    size_t ip = proc->ip;
    int* program = proc->new_file_buf;

    int first_el = 0;
    int sec_el   = 0;
    int arg      = 0;

    while(ip < size)
    {
        switch(program[ip])
        {
            case PUSH_A:
                proc->ip = ip;
                arg = get_arg(proc);
                stk_push (stk, arg, list);
                RETURN_VOID
                ip = proc->ip;
                break;

            case ADD_A:
                TWO_ARGS
                stk_push (stk, first_el + sec_el, list);
                RETURN_VOID
                break;

            case SUB_A:
                TWO_ARGS
                stk_push (stk, first_el - sec_el, list);
                RETURN_VOID
                break;

            case MUL_A:
                TWO_ARGS
                stk_push (stk, first_el * sec_el, list);
                RETURN_VOID
                break;

            case DIV_A:
                TWO_ARGS
                stk_push (stk, first_el / sec_el, list);
                RETURN_VOID
                break;

            case SQRT_A:
                ONE_ARG
                stk_push (stk, pow(arg, 0.5), list);
                RETURN_VOID
                break;
                
            case SIN_A:
                ONE_ARG
                stk_push (stk, sin(arg), list);
                RETURN_VOID
                break;

            case COS_A:
                ONE_ARG
                stk_push (stk, cos(arg), list);
                RETURN_VOID
                break;
            
            case JA_A:
                ip++;
                proc->ip = ip;
                ja(proc, stk, list);
                RETURN_VOID
                ip = proc->ip;
                ip--;
                break;

            case JAE_A:
                ip++;
                proc->ip = ip;
                jae(proc, stk, list);
                RETURN_VOID
                ip = proc->ip;
                ip--;
                break;

            case JB_A:
                ip++;
                proc->ip = ip;
                jb(proc, stk, list);
                RETURN_VOID
                ip = proc->ip;
                ip--;
                break;

            case JBE_A:
                ip++;
                proc->ip = ip;
                jbe(proc, stk, list);
                RETURN_VOID
                ip = proc->ip;
                ip--;
                break;

            case JE_A:
                ip++;
                proc->ip = ip;
                je(proc, stk, list);
                RETURN_VOID
                ip = proc->ip;
                ip--;
                break;

            case JNE_A:
                ip++;
                proc->ip = ip;
                jne(proc, stk, list);
                RETURN_VOID
                ip = proc->ip;
                ip--;
                break;

            case JMP_A:
                ip++;
                ip = proc->new_file_buf[ip];
                break;

            case POP_A:
                proc->ip = ip;
                stk_pop (stk, get_arg_addr(proc), list);
                RETURN_VOID
                ip = proc->ip;
                break;

            case HLT_A:
                ip = size + 1;
                break;

            case RET_A:
                ip = functions->data[ret_cnt];
                ret_cnt;
                break;

            default:
                //printf("something went wrong :(\n");
                break;
        }   
        ip++;
    }
}

int get_arg (Processor* proc)
{
    size_t ip = proc->ip;
    int* program = proc->new_file_buf;

    ip++;
    int arg = program[ip];
    int res_arg = 0;

    bool is_ram = false;

    if ((arg & RAM) == RAM)
        is_ram = true;

    if ((arg & INT) == INT)
    {
        res_arg += arg & FREE;
        ip++;
        arg = program[ip];
    }

    if ((arg & REG) == REG)
    {
        res_arg += registers[arg & FREE]->value;
    }
    else 
        ip--;

    if (is_ram == true)
        res_arg = proc->RAM[res_arg];

    proc->ip = ip;
    
    return res_arg;
}

int* get_arg_addr (Processor* proc)
{
    size_t ip = proc->ip;
    int* program = proc->new_file_buf;

    ip++;
    int* arg = &program[ip];

    int* res_addr = 0;
    int  res_arg  = 0;

    int for_stk_pop = 0;

    bool is_reg = false;
    bool is_ram = false;

    if (((*arg) & RAM) == RAM)
        is_ram = true;

    if (((*arg) & INT) == INT)
    {
        res_arg += (*arg) & FREE;
        ip++;
        *arg = program[ip];
    }

    if (((*arg) & REG) == REG)
    {
        is_reg = true;

        if (is_ram == true)
        {
            res_arg += registers[(*arg) & FREE]->value;
        }
        else
        {
            res_addr = &registers[(*arg) & FREE]->value;
        }
            
    }

    if (!is_reg && !is_ram)
    {
        ip--;
        res_addr = &for_stk_pop;
    }
        
    if (is_ram)
        res_addr = &proc->RAM[res_arg];

    proc->ip = ip;
    
    return res_addr;
}

void ja(Processor* proc, Stack* stk, ErrList *const list)
{
    size_t ip = proc->ip;
    int* program = proc->new_file_buf;
    int arg = program[ip];

    stack_element_t first_el = 0;
    stack_element_t sec_el = 0;

    stk_pop(stk, &sec_el, list);
    RETURN_VOID
    stk_pop(stk, &first_el, list);
    RETURN_VOID

    DoJump do_jump = NO_JUMP;
    
    ResultOfComparing res = comparing(first_el, sec_el);  //условная функция с кучей компараторов
    if (res == GREATER_RES)
        do_jump = DO_JUMP;

    if(do_jump == DO_JUMP)
        ip = arg;
    else 
        ip++;

    proc->ip = ip;
}

void jae (Processor* proc, Stack* stk, ErrList *const list)
{
    size_t ip = proc->ip;
    int* program = proc->new_file_buf;
    int arg = program[ip];
    
    stack_element_t first_el = 0;
    stack_element_t sec_el = 0;

    stk_pop(stk, &sec_el, list);
    RETURN_VOID
    stk_pop(stk, &first_el, list);
    RETURN_VOID

    DoJump do_jump = NO_JUMP;

    ResultOfComparing res = comparing(first_el, sec_el);
    if (res == GREATER_RES || res == EQUAL_RES)
        do_jump = DO_JUMP;

    if(do_jump == DO_JUMP)
        ip = arg;
    else 
        ip++;

    proc->ip = ip;
}

void jb(Processor* proc, Stack* stk, ErrList *const list)
{
    size_t ip = proc->ip;
    int* program = proc->new_file_buf;
    int arg = program[ip];

    stack_element_t first_el = 0;
    stack_element_t sec_el   = 0;

    stk_pop(stk, &sec_el, list);
    RETURN_VOID
    stk_pop(stk, &first_el, list);
    RETURN_VOID

    DoJump do_jump = NO_JUMP;

    ResultOfComparing res = comparing(first_el, sec_el);  //условная функция с кучей компараторов
    if (res == LESS_RES)
        do_jump = DO_JUMP;

    if(do_jump == DO_JUMP)
        ip = arg;
    else 
        ip++;

    proc->ip = ip;
}

void jbe (Processor* proc, Stack* stk, ErrList *const list)
{
    size_t ip = proc->ip;
    int* program = proc->new_file_buf;
    int arg = program[ip];
    
    stack_element_t first_el = 0;
    stack_element_t sec_el   = 0;

    stk_pop(stk, &sec_el, list);
    RETURN_VOID
    stk_pop(stk, &first_el, list);
    RETURN_VOID;

    DoJump do_jump = NO_JUMP;

    ResultOfComparing res = comparing(first_el, sec_el);
    if (res == LESS_RES || res == EQUAL_RES)
        do_jump = DO_JUMP;

    if(do_jump == DO_JUMP)
        ip = arg;
    else 
        ip++;

    proc->ip = ip;
}

void je (Processor* proc, Stack* stk, ErrList *const list)
{
    size_t ip = proc->ip;
    int* program = proc->new_file_buf;
    int arg = program[ip];
    
    stack_element_t first_el = 0;
    stack_element_t sec_el = 0;

    stk_pop(stk, &sec_el, list);
    RETURN_VOID
    stk_pop(stk, &first_el, list);
    RETURN_VOID

    DoJump do_jump = NO_JUMP;

    ResultOfComparing res = comparing(first_el, sec_el);
    if (res == EQUAL)
        do_jump = DO_JUMP;

    if(do_jump == DO_JUMP)
        ip = arg;
    else 
        ip++;

    proc->ip = ip;
}

void jne (Processor* proc, Stack* stk, ErrList *const list)
{
    size_t ip = proc->ip;
    int* program = proc->new_file_buf;
    int arg = program[ip];
    
    stack_element_t first_el = 0;
    stack_element_t sec_el = 0;

    stk_pop(stk, &sec_el, list);
    RETURN_VOID
    stk_pop(stk, &first_el, list);
    RETURN_VOID

    DoJump do_jump = NO_JUMP;

    ResultOfComparing res = comparing(first_el, sec_el);
    if (res == GREATER_RES || res == LESS_RES)
        do_jump = DO_JUMP;

    if(do_jump == DO_JUMP)
        ip = arg;
    else 
        ip++;

    proc->ip = ip;
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