#ifndef GSDDEF_H
#define GSDDEF_H

#include <vms_types.h>

#define GSD$K_LENGTH 49
#define GSD$C_LENGTH 49
#define GSD$K_EXTGSDLNG 61
#define GSD$C_EXTGSDLNG 61
#define GSD$S_GSDDEF 61
#define GSD$C_MAXNAMLEN 43

struct _gsd
{
    struct _gsd *gsd$l_gsdfl;
    struct _gsd *gsd$l_gsdbl;
    UINT16 gsd$w_size;
    UINT8 gsd$b_type;
    UINT8 gsd$b_fill;
    UINT32 gsd$l_hash;
    union
    {
        UINT32 gsd$l_pcbuic;
        struct
        {
            char gsddef$$_filler [2];
            UINT16 gsd$w_pcbgrp;
        };
    };
    UINT32 gsd$l_filuic;
    UINT32 gsd$l_prot;
    UINT32 gsd$l_gstx;
    UINT32 gsd$l_ident;
    struct _orb *gsd$l_orb;
    union
    {
        UINT32 gsd$l_ipid;
        UINT32 gsd$l_related_gstx;
    };
    UINT32 gsd$l_flags;
    union
    {
        struct
        {
            char gsd$t_gsdnam;
            INT8 gsd$b_fillme2 [3];
        };
        UINT32 gsd$l_basepfn;
    };
    UINT32 gsd$l_pages;
    UINT32 gsd$l_refcnt;
    char gsd$t_pfngsdnam;
    INT8 gsd$b_fillme [3];
};

#endif

