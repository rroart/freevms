#ifndef erl_routines_h
#define erl_routines_h

//#include "../../../freevms/lib/src/ints.h"
#include "../../../freevms/lib/src/embdvdef.h"
#include "../../../freevms/lib/src/ucbdef.h"

int   erl_std$allocemb (int size, struct _embdv **embdv_p);
void  erl_std$deviceattn (long long driver_param, struct _ucb *ucb);
void  erl_std$devicerr (long long driver_param, struct _ucb *ucb);
void  erl_std$devictmo (long long driver_param, struct _ucb *ucb);
void  erl_std$releasemb (struct _embdv *embdv);

#endif
