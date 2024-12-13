#ifndef _INPUT_H_
#define _INPUT_H_

#include "..\code_gen\code_gen.h"

static const char COMMENT_MARK = ';';
static const char SPACE = ' ';

void handle_text_argv (Input *const text, char **const argv, ErrList *const list);
void handle_text_wname (Input *const text, const char *const name, ErrList *const list);

#endif //_INPUT_H_