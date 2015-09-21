#ifndef IHADEF_H
#define IHADEF_H

#include <vms_types.h>

#define IHA$K_LENGTH 20
#define IHA$C_LENGTH 20
#define IHA$S_IHADEF 20

struct _iha
{
    UINT32 iha$l_tfradr1;
    UINT32 iha$l_tfradr2;
    UINT32 iha$l_tfradr3;
    INT32 ihadef$$_fill_1;
    UINT32 iha$l_inishr;
};

#endif

