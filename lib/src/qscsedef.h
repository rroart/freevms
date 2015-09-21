#ifndef QSCSEDEF_H
#define QSCSEDEF_H

#define QSCSE$M_SYNCH_PENDING 0x1
#define QSCSE$M_REBUILD 0x2

struct _qscse
{
    struct _qscse *qscse$ps_flink;
    struct _qscse *qscse$ps_blink;
    UINT16 qscse$w_size;
    UINT8 qscse$b_type;
    UINT8 qscse$b_subtype;
    UINT32 qscse$l_flags;
    void *qscse$ps_item;
    INT8 qscse$b_fill_0_ [4];
};

#define QSCSE$C_LENGTH 24
#define QSCSE$K_LENGTH 24

#define QSCSE$S_QSCSE 24

#endif

