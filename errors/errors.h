#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <assert.h>
#include <stdlib.h>

//----------------CONSTS--ENUMS----------------
static const size_t MAX_FILES_AMT      = 15;
static const size_t ERROR_VALUE_SIZE_T = 993;
static const int    ERROR_VALUE_INT    = -8;

enum Errors
{
    ALL_RIGHT,
    FILE_ERROR,
    READ_ERROR,
    WRITE_ERROR,
    CLOSE_ERROR,
    CMD_ERROR,
    ALLOCATION_ERROR,
    CPY_ERROR,
    STAT_ERROR,
    SPRINTF_ERROR,
    SYN_ERROR,
    MATH_ERROR,
    TOKEN_ERROR,
    RET_ERROR,
    SSCANF_ERROR
};

//------------STRUCTS------------------------
struct Err_param
{
    Errors err_num;

    const char* file;
    const char* func;
    int line;
};

struct ErrList
{
    Err_param* list;
    size_t head;
};

//-----------------DEFINES------------------

#define LOCATION_DEF const char *const file, const char *const func, int line
#define LOCATION __FILE__, __FUNCTION__, __LINE__ 

//=================MAKE--ERR==========================

#define ERROR(err_num)  list_push(list, LOCATION, err_num);

#define FILE_CHECK(file)    do                                                  \
                            {                                                   \
                                if (file == nullptr)                            \
                                {                                               \
                                    ERROR(FILE_ERROR)                           \
                                    return;                                     \
                                }                                               \
                            }while(0);

#define STAT_CHECK(name)        do                                              \
                                {                                               \
                                    if (stat(name, &file_info) == -1)           \
                                    {                                           \
                                        ERROR(STAT_ERROR)                       \
                                        return;                                 \
                                    }                                           \
                                }while(0);

#define CMD_CHECK               do                                              \
                                {                                               \
                                    if (system_res != 0)                        \
                                    {                                           \
                                        ERROR(CMD_ERROR)                        \
                                        return;                                 \
                                    }                                           \
                                }while(0);

#define ALLOCATION_CHECK_PTR(buf)   do                                          \
                                    {                                           \
                                        if (buf == nullptr)                     \
                                        {                                       \
                                            ERROR(ALLOCATION_ERROR)             \
                                            return nullptr;                     \
                                        }                                       \
                                    }while(0);

#define ALLOCATION_CHECK_VOID(buf) do                                           \
                            {                                                   \
                                if (buf == 0)                                   \
                                {                                               \
                                    ERROR(ALLOCATION_ERROR)                     \
                                    return;                                     \
                                }                                               \
                            }while(0);

#define SPRINTF_CHECK_VOID do                                                   \
                            {                                                   \
                                if (sprintf_res == -1)                          \
                                {                                               \
                                    ERROR(SPRINTF_ERROR)                        \
                                    return;                                     \
                                }                                               \
                            }while(0);

#define SPRINTF_CHECK_PTR do                                                    \
                            {                                                   \
                                if (sprintf_res == -1)                          \
                                {                                               \
                                    ERROR(SPRINTF_ERROR)                        \
                                    return nullptr;                             \
                                }                                               \
                            }while(0);

#define CPY_CHECK(name)     do                                                  \
                            {                                                   \
                                if(name == nullptr)                             \
                                {                                               \
                                    ERROR(CPY_ERROR)                            \
                                    return;                                     \
                                }                                               \
                            }while(0);

#define READ_CHECK           do                                                 \
                            {                                                   \
                                if (read_result != size)                        \
                                {                                               \
                                    ERROR(READ_ERROR)                           \
                                    return;                                     \
                                }                                               \
                            }while(0);

#define WRITE_CHECK     do                                                      \
                        {                                                       \
                            if (write_res == 0)                                 \
                            {                                                   \
                                ERROR(WRITE_ERROR)                              \
                                return;                                         \
                            }                                                   \
                        }while(0);

#define CLOSE_CHECK         do                                                  \
                            {                                                   \
                                if(close_res != 0)                              \
                                {                                               \
                                    ERROR(CLOSE_ERROR)                          \
                                    return;                                     \
                                }                                               \
                            }while(0);

#define SSCANF_CHECK    do                                 \
                        {                                  \
                            if (sscanf_res == -1)        \
                            {                              \
                                ERROR(SSCANF_ERROR)       \
                                return;                     \
                            }                              \
                        }while(0);

//=================RET================================


#define MAIN           do                                                                                                               \
                        {                                                                                                               \
                            if (list.head != 0)                                                                                         \
                            {                                                                                                           \
                                printf("you have problem number %d\n", list.list[0].err_num);                                           \
                                for (int i = 0; i < list.head; i++)                                                                     \
                                    printf("in file %s, func %s, line %d\n", list.list[i].file, list.list[i].func, list.list[i].line);  \
                                return 1;                                                                                               \
                            }                                                                                                           \
                        } while (0);    

#define RETURN_VOID     do                                                      \
                        {                                                       \
                            if (list->head != 0)                                \
                            {                                                   \
                                ERROR(RET_ERROR)                                \
                                return;                                         \
                            }                                                   \
                        }while(0);

#define RETURN_PTR      do                                                      \
                        {                                                       \
                            if (list->head != 0)                                \
                            {                                                   \
                                ERROR(RET_ERROR)                                \
                                return nullptr;                                 \
                            }                                                   \
                        }while(0);

#define RETURN_SIZE_T   do                                                      \
                        {                                                       \
                            if (list->head != 0)                                \
                            {                                                   \
                                ERROR(RET_ERROR)                                \
                                return ERROR_VALUE_SIZE_T;                      \
                            }                                                   \
                        }while(0);

#define RETURN_BOOL     do                                                      \
                        {                                                       \
                            if (list->head != 0)                                \
                            {                                                   \
                                ERROR(RET_ERROR)                                \
                                return false;                                   \
                            }                                                   \
                        }while(0);



//-----------------FUNCS-----------------

void error_list_ctor(ErrList *const list);
void list_push(ErrList *const list, LOCATION_DEF, Errors err);
void error_list_dtor(ErrList *const list);


#endif //_ERRORS_H_