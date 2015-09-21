#ifndef PLGDEF_H
#define PLGDEF_H

#include <vms_types.h>

#define PLG$M_NOEXTEND 0x1
#define PLG$C_VER_NO 1
#define PLG$C_VER_IDX 2
#define PLG$C_VER_3 3
#define PLG$K_BLN 122
#define PLG$C_BLN 122
#define PLG$S_PLGDEF 122

// like Relative Files Prologue Block in rmsint2.doc?

struct _plg
{
    INT8 plg$$_fill_1 [11];
    UINT8 plg$b_dbktsiz;
    INT32 plg$$_fill_2;
    union
    {
        UINT8 plg$b_flags;
        struct
        {
            unsigned plg$v_noextend : 1;
            unsigned plg$v_fill_2_ : 7;
        };
    };
    INT8 plg$$_fill_3 [85];
    UINT8 plg$b_avbn;
    UINT8 plg$b_amax;
    UINT16 plg$w_dvbn;
    INT16 plg$$_fill_4;
    UINT32 plg$l_mrn;
    UINT32 plg$l_eof;
    UINT16 plg$w_ver_no;
    UINT16 plg$w_gbc;
    UINT16 plg$w_colvbn;
};

#endif

