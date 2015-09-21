#ifndef BODDEF_H
#define BODDEF_H

#include <vms_types.h>

#define BOD$M_DELPEN    0x1
#define BOD$M_NOQUOTA   0x2
#define BOD$M_S2_WINDOW 0x4
#define BOD$M_NOSVA     0x8
#define BOD$M_SYSBUFOBJ 0x10

#define BOD$K_LENGTH 64
#define BOD$C_LENGTH 64
#define BOD$S_BODDEF 64

struct _bod
{
    struct _bod *bod$l_flink;
    struct _bod *bod$l_blink;
    UINT16 bod$w_size;
    UINT8 bod$b_type;
    UINT8 bod$b_align_1;
    UINT32 bod$l_acmode;
    UINT32 bod$l_seqnum;
    UINT32 bod$l_refcnt;
    union
    {
        UINT32 bod$l_flags;
        struct
        {
            unsigned bod$v_delpen    : 1;
            unsigned bod$v_noquota   : 1;
            unsigned bod$v_s2_window : 1;
            unsigned bod$v_nosva     : 1;
            unsigned bod$v_sysbufobj : 1;
            unsigned bod$v_fill_0_   : 3;
        };
    };
    UINT32 bod$l_pid;
    void *bod$pq_basepva;
    union
    {
        void *bod$pq_basesva;
        void *bod$l_basesva;
    };
    union
    {
        struct _pte *bod$pq_va_pte;
        struct _pte *bod$ps_svapte;
    };
    UINT32 bod$l_pagcnt;
    INT8 bod$b_fill_1_ [4];
};

#endif

