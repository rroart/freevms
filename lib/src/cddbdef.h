#ifndef CDDBDEF_H
#define CDDBDEF_H

#include <vms_types.h>

#define     CDDB$M_SNGLSTRM     0x1
#define     CDDB$M_IMPEND       0x2
#define     CDDB$M_INITING      0x4
#define     CDDB$M_RECONNECT    0x8
#define     CDDB$M_RESYNCH      0x10
#define     CDDB$M_POLLING      0x20
#define     CDDB$M_ALCLS_SET    0x40
#define     CDDB$M_NOCONN       0x80
#define     CDDB$M_RSTRTWAIT    0x100
#define     CDDB$M_QUORLOST     0x200
#define     CDDB$M_DAPBSY       0x400
#define     CDDB$M_2PBSY        0x800
#define     CDDB$M_BSHADOW      0x1000
#define     CDDB$M_DISABLED     0x2000
#define     CDDB$M_PATHMOVE     0x4000
#define     CDDB$M_PRMBSY       0x8000
#define     CDDB$M_DISC_PEND    0x10000
#define     CDDB$M_CRNSET       0x20000
#define     CDDB$M_FIRM_WREV    0x40000
#define     CDDB$M_PRMBSY_CLEANUP_PERMITTED     0x80000
#define     CDDB$K_LENGTH       152
#define     CDDB$C_LENGTH       152

#define     CDDB$S_CDDBDEF      152

struct _cddb
{
    struct _cdrp *cddb$l_cdrpqfl;
    struct _cdrp *cddb$l_cdrpqbl;
    UINT16 cddb$w_size;
    UINT8 cddb$b_type;
    UINT8 cddb$b_subtype;
    UINT8 cddb$b_systemid [8];
    union
    {
        UINT32 cddb$l_status;
        struct
        {
            unsigned cddb$v_snglstrm    : 1;
            unsigned cddb$v_impend  : 1;
            unsigned cddb$v_initing : 1;
            unsigned cddb$v_reconnect   : 1;
            unsigned cddb$v_resynch : 1;
            unsigned cddb$v_polling : 1;
            unsigned cddb$v_alcls_set   : 1;
            unsigned cddb$v_noconn  : 1;
            unsigned cddb$v_rstrtwait   : 1;
            unsigned cddb$v_quorlost    : 1;
            unsigned cddb$v_dapbsy  : 1;
            unsigned cddb$v_2pbsy   : 1;
            unsigned cddb$v_bshadow : 1;
            unsigned cddb$v_disabled    : 1;
            unsigned cddb$v_pathmove    : 1;
            unsigned cddb$v_prmbsy  : 1;
            unsigned cddb$v_disc_pend   : 1;
            unsigned cddb$v_crnset  : 1;
            unsigned cddb$v_firm_wrev : 1;
            unsigned cddb$v_prmbsy_cleanup_permitted : 1;
            unsigned cddb$v_fill_0_ : 4;
        };
    };
    struct _pdt *cddb$l_pdt;
    struct _crb *cddb$l_crb;
    struct _ddb *cddb$l_ddb;
    union
    {
        UINT64 cddb$q_cntrlid;
        struct
        {
            INT8 cddbdef$$_fill_2 [6];
            UINT8 cddb$b_cntrlmdl;
            UINT8 cddb$b_cntrlcls;
        };
    };
    UINT16 cddb$w_cntrlflgs;
    UINT16 cddb$w_cntrltmo;
    UINT32 cddb$l_oldrspid;
    UINT32 cddb$l_oldcmdsts;
    struct _cdrp *cddb$l_rstrtcdrp;
    UINT32 cddb$l_retrycnt;
    UINT32 cddb$l_dapcount;
    UINT32 cddb$l_rstrtcnt;
    struct _cdrp *cddb$l_rstrtqfl;
    struct _cdrp *cddb$l_rstrtqbl;
    void (*cddb$l_saved_pc)(void);
    void (*cddb$l_saved_pc1)(void);
    struct _ucb *cddb$l_ucbchain;
    struct _ucb *cddb$l_origucb;
    UINT32 cddb$l_allocls;
    struct _cdrp *cddb$l_dapcdrp;
    struct _cddb *cddb$l_cddblink;
    UINT32 cddb$l_fover_ctr;
    UINT32 cddb$l_wtucbctr;
    UINT32 cddb$l_maxbcnt;
    UINT32 cddb$l_ctrltr_mask;
    UINT32 cddb$l_cpyseqnum;
    UINT32 cddb$l_dap_limit;
    UINT8 cddb$b_csvrsn;
    UINT8 cddb$b_chvrsn;
    UINT16 cddb$w_load_avail;
    UINT32 cddb$l_counter;
    UINT32 cddb$l_conid;
    UINT32 cddb$l_cdt;
    struct _cdrp *cddb$l_prmcdrp;
};

#endif

