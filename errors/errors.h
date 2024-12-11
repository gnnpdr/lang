#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <assert.h>

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
    TOKEN_ERROR
};

struct Err_param
{
    Errors err_num;

    const char* file;
    const char* func;
    int line;
};

#define LOCATION_DEF const char *const file, const char *const func, int line
#define LOCATION __FILE__, __FUNCTION__, __LINE__ 

#define ERROR_CTOR  Err_param error = {};       \
	                error.err_num = ALL_RIGHT;  


#define ERROR(err_num)  do                                                          \
                        {                                                           \
                            fill_error(error, LOCATION, err_num);                   \
                        } while (0);

#define MAIN_CHECK           do                                                                                    \
                        {                                                                                       \
                            if (error.err_num != ALL_RIGHT)                                                     \
                            {                                                                                   \
                                printf("you have problem number %d", error.err_num);                            \
                                printf("in file %s, func %s, line %d\n", error.file, error.func, error.line);   \
                                return 1;                                                                         \
                            }                                                                                   \
                        } while (0);    

#define RETURN_VOID     do                                      \
                        {                                       \
                            if (error->err_num != ALL_RIGHT)    \
                                return;                         \
                        }while(0);

#define RETURN_PTR      do                                      \
                        {                                       \
                            if (error->err_num != ALL_RIGHT)    \
                                return nullptr;                 \
                        }while(0);

#define RETURN_SIZE_T   do                                      \
                        {                                       \
                            if (error->err_num != ALL_RIGHT)    \
                                return 1;                       \
                        }while(0);

#define RETURN_BOOL     do                                      \
                        {                                       \
                            if (error->err_num != ALL_RIGHT)    \
                                return false;                   \
                        }while(0);

#define FILE_CHECK(file)    do                                      \
                            {                                       \
                                if (file == nullptr)                \
                                {                                   \
                                    ERROR(FILE_ERROR)               \
                                    return;                         \
                                }                                   \
                            }while(0);

#define STAT_CHECK(name)        do                                      \
                                {                                       \
                                    if (stat(name, &file_info) == -1)   \
                                    {                                   \
                                        ERROR(STAT_ERROR)               \
                                        return;                         \
                                    }                                   \
                                }while(0);


#define CMD_CHECK               do                                       \
                                {                                       \
                                    if (system_res != 0)                \
                                    {                                   \
                                        ERROR(CMD_ERROR)                \
                                        return;                         \
                                    }                                   \
                                }while(0);

#define ALLOCATION_CHECK_RET(buf)   do                                      \
                                    {                                       \
                                        if (buf == nullptr)                 \
                                        {                                   \
                                            ERROR(ALLOCATION_ERROR)         \
                                            return nullptr;                 \
                                        }                                   \
                                    }while(0);

#define ALLOCATION_CHECK(buf) do                                    \
                            {                                       \
                                if (buf == 0)                       \
                                {                                   \
                                    ERROR(ALLOCATION_ERROR)         \
                                    return;                         \
                                }                                   \
                            }while(0);

#define SPRINTF_CHECK do                                            \
                            {                                       \
                                if (sprintf_res == -1)              \
                                {                                   \
                                    ERROR(SPRINTF_ERROR)            \
                                    return;                         \
                                }                                   \
                            }while(0);

#define SPRINTF_CHECK_NULL do                                       \
                            {                                       \
                                if (sprintf_res == -1)              \
                                {                                   \
                                    ERROR(SPRINTF_ERROR)            \
                                    return nullptr;                 \
                                }                                   \
                            }while(0);

#define CPY_CHECK(name)     do                                   \
                            {                                    \
                                if(name == nullptr)              \
                                {                                \
                                    ERROR(CPY_ERROR)             \
                                    return;                      \
                                }                                \
                            }while(0);

#define READ_CHECK           do                                 \
                            {                                   \
                                if (read_result != size)        \
                                {                               \
                                    ERROR(READ_ERROR)           \
                                    return;                     \
                                }                               \
                            }while(0);

#define WRITE_CHECK     do                                 \
                        {                                  \
                            if (write_res == 0)            \
                            {                              \
                                ERROR(WRITE_ERROR)         \
                                return;                    \
                            }                              \
                        }while(0);

#define CLOSE_CHECK           do                                \
                            {                                   \
                                if(close_res != 0)              \
                                {                               \
                                    ERROR(CLOSE_ERROR)          \
                                    return;                     \
                                }                               \
                            }while(0);

void fill_error(Err_param *const error, LOCATION_DEF, Errors err);

#endif //_ERRORS_H_