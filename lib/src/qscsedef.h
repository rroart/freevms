#ifndef qscsedef_h
#define qscsedef_h

#define QSCSE$M_SYNCH_PENDING 0x1
#define QSCSE$M_REBUILD 0x2

struct _qscse
{
    struct _qscse *qscse$ps_flink;
    struct _qscse *qscse$ps_blink;
    unsigned short int qscse$w_size;
    unsigned char qscse$b_type;
    unsigned char qscse$b_subtype;
    unsigned int qscse$l_flags;
    void *qscse$ps_item;
    char qscse$b_fill_0_ [4];
};

#define QSCSE$C_LENGTH 24
#define QSCSE$K_LENGTH 24

#define QSCSE$S_QSCSE 24

#endif

