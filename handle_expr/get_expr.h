#ifndef _MIDEND_H_
#define _MIDEND_H_

#include <math.h>
#include "syn_analysis.h"


Node* analyse_text(Token *const tokens, Id *const ids, Input *const base_text, ErrList *const list);


#endif //_MIDEND_H_