#ifndef TQEDEF_H
#define TQEDEF_H

#include <vms_types.h>

#define TQE$M_TQTYPE 0x3
#define TQE$M_REPEAT 0x4
#define TQE$M_ABSOLUTE 0x8
#define TQE$M_CHK_CPUTIM 0x10
#define TQE$M_EXTENDED_FORMAT 0x20
#define TQE$M_RSRVD_6_7 0xC0
#define TQE$C_TMSNGL 0
#define TQE$C_SSREPT 5
#define TQE$C_SSSNGL 1
#define TQE$C_WKREPT 6
#define TQE$C_WKSNGL 2
#define TQE$S_TQEDEF 64

#define TQE$K_LENGTH 64
#define TQE$C_LENGTH 64

struct _tqe
{
    struct _tqe *tqe$l_tqfl;
    struct _tqe *tqe$l_tqbl;
    UINT16 tqe$w_size;
    UINT8 tqe$b_type;
    UINT8 tqe$b_rqtype;
    union
    {
        unsigned long tqe$l_pid;
        long tqe$l_fpc;
    };
    union
    {
        struct
        {
            void (*tqe$l_ast)(unsigned long);
            unsigned long tqe$l_astprm;
        };
        struct
        {
            unsigned long tqe$l_fr3;
            unsigned long tqe$l_fr4;
        };
    };
    UINT64 tqe$q_time;
    UINT64 tqe$q_delta;
    UINT8 tqe$b_rmod;
    UINT8 tqe$b_efn;
    UINT16 tqe$w_res;
    UINT32 tqe$l_rqpid;
    UINT32 tqe$l_cputim;
};

#endif

