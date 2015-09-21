#ifndef FI5DEF_H
#define FI5DEF_H

#include <vms_types.h>

#define FI5$C_ODS2 0
#define FI5$C_ISL1 1
#define FI5$$_TYPE_RESERVED 2
#define FI5$C_UCS2 3
#define FI5$M_FIXED_LENGTH 0x10
#define FI5$C_MIN_LENGTH 120
#define FI5$C_MAX_LENGTH 324
#define FI5$S_FI5DEF 324

struct _fi5
{
    union
    {
        UINT8 fi5$b_control;
#if 0
        // aaah damn bugger bugs. seems control_fields counts as 2 bytes
        // and b_namelen as 2
        struct
        {
            unsigned fi5$v_nametype : 2;
            unsigned fi5$$_fill_1 : 2;
            unsigned fi5$v_fixed_length : 1;
            unsigned fi5$v_fill_2_ : 3;
        } fi5$r_control_fields;
#endif
    } fi5$r_control_overlay;
    UINT8 fi5$b_namelen;
    UINT16 fi5$w_revision;
    UINT64 fi5$q_credate;
    UINT64 fi5$q_revdate;
    UINT64 fi5$q_expdate;
    UINT64 fi5$q_bakdate;
    UINT64 fi5$q_accdate;
    UINT64 fi5$q_attdate;
    UINT64 fi5$q_ex_recattr;
    struct
    {
        UINT64 fi5$q_hint_lo_qw;
        UINT64 fi5$q_hint_hi_qw;
    } fi5$r_length_hint;
    char fi5$t_filename [44];
    char fi5$t_filenamext [204];
};

#define     fi5$b_control       fi5$r_control_overlay.fi5$b_control
#define     fi5$q_hint_hi_qw    fi5$r_length_hint.fi5$q_hint_hi_qw
#define     fi5$q_hint_lo_qw    fi5$r_length_hint.fi5$q_hint_lo_qw
#define     fi5$v_fixed_length  fi5$r_control_overlay.fi5$r_control_fields.fi5$v_fixed_length
#define     fi5$v_nametype      fi5$r_control_overlay.fi5$r_control_fields.fi5$v_nametype

#endif

