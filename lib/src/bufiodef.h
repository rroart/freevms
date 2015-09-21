#ifndef BUFIODEF_H
#define BUFIODEF_H

#include <vms_types.h>

#define BUFIO$K_64 -1
#define BUFIO$K_HDRLEN32 12
#define BUFIO$K_HDRLEN64 24

struct _bufio
{
    void *bufio$ps_pktdata;
    void *bufio$ps_uva32;
    UINT16 bufio$w_size;
    UINT8 bufio$b_type;
    INT8 bufio$b_fill_1;
    INT32 bufio$l_fill_2;
    void *bufio$pq_uva64;
};

#endif

