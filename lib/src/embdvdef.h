#ifndef EMBDVDEF_H
#define EMBDVDEF_H

#include <embhddef.h>

#define EMB$K_DV_LENGTH 222
#define EMB$C_DV_LENGTH 222
#define EMB$S_EMBDVDEF 222

struct _embdv
{
    unsigned int emb$l_dv_sid;
    unsigned short int emb$w_dv_hdr_rev;
    unsigned int emb$l_dv_xsid;
    unsigned int emb$l_dv_cpuid;
    unsigned char emb$b_dv_dev_class;
    unsigned char emb$b_dv_dev_type;
    SCS_NAME_LEN emb$b_dv_scs_name;
    unsigned short int emb$w_dv_flags;
    unsigned char emb$b_dv_os_id;
    unsigned char emb$b_dv_hdrsz;
    unsigned short int emb$w_dv_entry;
    unsigned long long emb$q_dv_time;
    unsigned short int emb$w_dv_errseq;
    unsigned long long emb$q_dv_swvers;
    unsigned int emb$l_dv_errmsk;
    unsigned int emb$l_dv_abstim;
    unsigned char emb$b_dv_hw_name_len;
    char emb$t_dv_hw_name [31];
    unsigned int emb$l_dv_ertcnt;
    unsigned int emb$l_dv_ertmax;
    unsigned long long emb$q_dv_iosb;
    unsigned int emb$l_dv_sts;
    unsigned char emb$b_dv_class;
    unsigned char emb$b_dv_type;
    unsigned int emb$l_dv_rqpid;
    unsigned int emb$l_dv_boff;
    unsigned int emb$l_dv_bcnt;
    void *emb$l_dv_media;
    unsigned short int emb$w_dv_unit;
    unsigned int emb$l_dv_errcnt;
    unsigned int emb$l_dv_opcnt;
    unsigned int emb$l_dv_ownuic;
    unsigned int emb$l_dv_char;
    unsigned char emb$b_dv_slave;
    char embdv$$_fill_1;
    unsigned int emb$l_dv_func;
    char emb$t_dv_name [32];
    char emb$t_dv_dtname [28];
    void *emb$l_dv_regsav;
};

#endif

