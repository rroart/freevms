#ifndef EMBDVDEF_H
#define EMBDVDEF_H

#include <embhddef.h>

#define EMB$K_DV_LENGTH 222
#define EMB$C_DV_LENGTH 222
#define EMB$S_EMBDVDEF 222

struct _embdv
{
    UINT32 emb$l_dv_sid;
    UINT16 emb$w_dv_hdr_rev;
    UINT32 emb$l_dv_xsid;
    UINT32 emb$l_dv_cpuid;
    UINT8 emb$b_dv_dev_class;
    UINT8 emb$b_dv_dev_type;
    SCS_NAME_LEN emb$b_dv_scs_name;
    UINT16 emb$w_dv_flags;
    UINT8 emb$b_dv_os_id;
    UINT8 emb$b_dv_hdrsz;
    UINT16 emb$w_dv_entry;
    UINT64 emb$q_dv_time;
    UINT16 emb$w_dv_errseq;
    UINT64 emb$q_dv_swvers;
    UINT32 emb$l_dv_errmsk;
    UINT32 emb$l_dv_abstim;
    UINT8 emb$b_dv_hw_name_len;
    char emb$t_dv_hw_name [31];
    UINT32 emb$l_dv_ertcnt;
    UINT32 emb$l_dv_ertmax;
    UINT64 emb$q_dv_iosb;
    UINT32 emb$l_dv_sts;
    UINT8 emb$b_dv_class;
    UINT8 emb$b_dv_type;
    UINT32 emb$l_dv_rqpid;
    UINT32 emb$l_dv_boff;
    UINT32 emb$l_dv_bcnt;
    void *emb$l_dv_media;
    UINT16 emb$w_dv_unit;
    UINT32 emb$l_dv_errcnt;
    UINT32 emb$l_dv_opcnt;
    UINT32 emb$l_dv_ownuic;
    UINT32 emb$l_dv_char;
    UINT8 emb$b_dv_slave;
    INT8 embdv$$_fill_1;
    UINT32 emb$l_dv_func;
    char emb$t_dv_name [32];
    char emb$t_dv_dtname [28];
    void *emb$l_dv_regsav;
};

#endif

