#ifndef PHDDEF_H
#define PHDDEF_H

#include <vms_types.h>

#define PHD$M_ASTEN 0xF
#define PHD$M_ASTSR 0xF0
#define PHD$M_ASTEN_KEN 0x1
#define PHD$M_ASTEN_EEN 0x2
#define PHD$M_ASTEN_SEN 0x4
#define PHD$M_ASTEN_UEN 0x8
#define PHD$M_ASTSR_KPD 0x10
#define PHD$M_ASTSR_EPD 0x20
#define PHD$M_ASTSR_SPD 0x40
#define PHD$M_ASTSR_UPD 0x80
#define PHD$M_FEN 0x1
#define PHD$M_PME 0x4000000000000000
#define PHD$M_DATFX 0x8000000000000000
#define PHD$C_HWPCBLEN 128
#define PHD$K_HWPCBLEN 128
#define PHD$C_FPR_COUNT 32
#define PHD$K_FPR_COUNT 32
#define PHD$M_SW_FEN 0x1
#define PHD$M_AST_PENDING 0x80000000
#define PHD$M_PFMFLG 0x1
#define PHD$M_DALCSTX 0x2
#define PHD$M_WSPEAKCHK 0x4
#define PHD$M_NOACCVIO 0x8
#define PHD$M_IWSPEAKCK 0x10
#define PHD$M_IMGDMP 0x20
#define PHD$M_NO_WS_CHNG 0x40
#define PHD$M_SPARE_8 0x80
#define PHD$M_LOCK_HEADER 0x100
#define PHD$M_FREWSLE_ACTIVE 0x200
#define PHD$K_LENGTH 1008
#define PHD$C_LENGTH 1008

struct _phd
{
    UINT64 phd$q_privmsk;
    UINT16 phd$w_size;
    UINT8 phd$b_type;
    INT8 phd$$_spare_1;
#ifdef __i386__
#define PHD_INT_SIZE 4
    UINT32 phd$l_wslist;
    UINT32 phd$l_wslock;
    UINT32 phd$l_wsdyn;
    UINT32 phd$l_wsnext;
    UINT32 phd$l_wslast;
    UINT32 phd$l_wsextent;
    UINT32 phd$l_wsquota;
    UINT32 phd$l_dfwscnt;
    UINT32 phd$l_cpulim;
    UINT32 phd$l_pst_base_offset;
    UINT32 phd$l_pst_last;
    UINT32 phd$l_pst_free;
    UINT32 phd$l_iorefc;
    UINT64 phd$q_next_region_id;
    INT32 phd$$_spare_2;
    UINT32 phd$l_emptpg;
    UINT32 phd$l_dfpfc;
    UINT32 phd$l_pgtbpfc;
    UINT32 phd$l_astlm;
#else
    // these are relative offset, not use yet
#define PHD_INT_SIZE 8
    unsigned long phd$l_wslist;
    unsigned long phd$l_wslock;
    unsigned long phd$l_wsdyn;
    unsigned long phd$l_wsnext;
    unsigned long phd$l_wslast;
    unsigned long phd$l_wsextent;
    unsigned long phd$l_wsquota;
    unsigned long phd$l_dfwscnt;
    unsigned long phd$l_cpulim;
    unsigned long phd$l_pst_base_offset;
    unsigned long phd$l_pst_last;
    unsigned long phd$l_pst_free;
    unsigned long phd$l_iorefc;
    UINT64 phd$q_next_region_id;
    long phd$$_spare_2;
    unsigned long phd$l_emptpg;
    unsigned long phd$l_dfpfc;
    unsigned long phd$l_pgtbpfc;
    unsigned long phd$l_astlm;
#endif
    union
    {
        INT32 phd$l_pst_base_max;
        INT32 phd$l_fredoff;
        INT32 phd$l_im_semaphore;
    };
    UINT32 phd$l_wssize;
    UINT32 phd$l_diocnt;
    UINT32 phd$l_biocnt;
    UINT32 phd$l_phvindex;
    INT64 phd$$_spare_18;
    union
    {
        UINT64 phd$q_lefc;
        struct
        {
            UINT32 phd$l_lefc_0;
            UINT32 phd$l_lefc_1;
        };
    };
    union
    {
        UINT64 phd$q_hwpcb;
        UINT64 phd$q_ksp;
    };
    UINT64 phd$q_esp;
    UINT64 phd$q_ssp;
    UINT64 phd$q_usp;
    UINT64 phd$q_ptbr;
    UINT64 phd$q_asn;
    union
    {
        UINT64 phd$q_astsr_asten;
        struct
        {
            unsigned phd$v_asten : 4;
            unsigned phd$v_astsr : 4;
        };
        struct
        {
            unsigned phd$v_asten_ken : 1;
            unsigned phd$v_asten_een : 1;
            unsigned phd$v_asten_sen : 1;
            unsigned phd$v_asten_uen : 1;
            unsigned phd$v_astsr_kpd : 1;
            unsigned phd$v_astsr_epd : 1;
            unsigned phd$v_astsr_spd : 1;
            unsigned phd$v_astsr_upd : 1;
        };
    };
    union
    {
        UINT64 phd$q_fen_datfx;
        struct
        {
            unsigned phd$v_fen : 1;
            unsigned phd$v_fill_61_1 : 32;
            unsigned phd$v_fill_61_2 : 29;
            unsigned phd$v_pme : 1;
            unsigned phd$v_datfx : 1;
        };
    };
    UINT64 phd$q_cc;
    UINT64 phd$q_unq;
    INT64 phd$q_pal_rsvd [6];
    union
    {
        INT64 phd$q_fpr [32];
        struct
        {
            UINT64 phd$q_f0;
            UINT64 phd$q_f1;
            UINT64 phd$q_f2;
            UINT64 phd$q_f3;
            UINT64 phd$q_f4;
            UINT64 phd$q_f5;
            UINT64 phd$q_f6;
            UINT64 phd$q_f7;
            UINT64 phd$q_f8;
            UINT64 phd$q_f9;
            UINT64 phd$q_f10;
            UINT64 phd$q_f11;
            UINT64 phd$q_f12;
            UINT64 phd$q_f13;
            UINT64 phd$q_f14;
            UINT64 phd$q_f15;
            UINT64 phd$q_f16;
            UINT64 phd$q_f17;
            UINT64 phd$q_f18;
            UINT64 phd$q_f19;
            UINT64 phd$q_f20;
            UINT64 phd$q_f21;
            UINT64 phd$q_f22;
            UINT64 phd$q_f23;
            UINT64 phd$q_f24;
            UINT64 phd$q_f25;
            UINT64 phd$q_f26;
            UINT64 phd$q_f27;
            UINT64 phd$q_f28;
            UINT64 phd$q_f29;
            UINT64 phd$q_f30;
            UINT64 phd$q_fpcr;
        };
    };
    union
    {
        UINT32 phd$l_flags2;
        struct
        {
            unsigned phd$v_sw_fen : 1;
            unsigned phd$v_fill_flags2 : 30;
            unsigned phd$v_ast_pending : 1;
        };
    };
    UINT32 phd$l_extracpu;
    UINT64 phd$q_asnseq;
    UINT32 phd$l_extdynws;
    UINT32 phd$l_pageflts;
    UINT32 phd$l_fow_flts;
    UINT32 phd$l_for_flts;
    UINT32 phd$l_foe_flts;
    UINT32 phd$l_cputim;
    UINT32 phd$l_cpumode;
    UINT32 phd$l_awsmode;
    UINT32 phd$$_spare_16;
    UINT32 phd$$_spare_17;
    UINT32 phd$l_ptcntlck;
    UINT32 phd$l_ptcntval;
    UINT32 phd$l_ptcntact;
    UINT32 phd$l_ptcntmax;
    UINT32 phd$$_spare_12;
    UINT32 phd$$_spare_14;
    UINT8 phd$$_spare_15 [4];
    INT32 phd$$_spare_3;
    INT32 phd$$_spare_4;
    INT32 phd$l_wsfluid;
    UINT32 phd$l_wsauth;
    UINT32 phd$l_wsauthext;
    void *phd$l_reslsth;
    UINT32 phd$l_authpri;
    UINT64 phd$q_authpriv;
    UINT64 phd$q_imagpriv;
    UINT32 phd$l_imgcnt;
    UINT32 phd$l_pfltrate;
    UINT32 phd$l_pflref;
    UINT32 phd$l_timref;
    UINT32 phd$l_pgfltio;
    struct
    {
        UINT8 phd$$$_fill_3 [20];
    };
    struct
    {
        UINT8 phd$$$_fill_4 [20];
    };
    UINT32 phd$$_spare_11 [4];
    INT32 phd$$_spare_10;
    union
    {
        INT64 phd$q_pagefile_refs;
        struct
        {
            INT32 phd$l_pagefile_refs_lo;
            INT32 phd$l_pagefile_refs_hi;
        };
    };
    INT32 phd$$_spare_13;
    UINT32 phd$$_spare_9;

    union
    {
        UINT32 phd$l_flags;
        struct
        {
            unsigned phd$v_pfmflg : 1;
            unsigned phd$v_dalcstx : 1;
            unsigned phd$v_wspeakchk : 1;
            unsigned phd$v_noaccvio : 1;
            unsigned phd$v_iwspeakck : 1;
            unsigned phd$v_imgdmp : 1;
            unsigned phd$v_no_ws_chng : 1;
            unsigned phd$$_spare_8 : 1;
            unsigned phd$v_lock_header : 1;
            unsigned phd$v_frewsle_active : 1;
            unsigned phd$v_fill_0_ : 6;
        };
    };
    UINT32 phd$l_pscanctx_seqnum;
    UINT64 phd$q_pscanctx_queue;
    INT32 phd$l_l2pt_wslx;
    INT32 phd$l_l3pt_wslx;
    INT32 phd$l_l3pt_count;
    INT32 phd$l_l2pt_count;
    INT32 phd$l_bufobj_wslx;
    INT32 phd$$_spare_5;
    INT32 phd$$_spare_6;
    INT32 phd$$_spare_7;
    void *phd$pq_pt_no_delete1;
    void *phd$pq_pt_no_delete2;
    UINT64 phd$q_free_pte_count;
    union
    {
        INT64 phd$q_p0_rde;
        struct
        {
            struct _rde *phd$ps_p0_va_list_flink;
            struct _rde *phd$ps_p0_va_list_blink;
        };
    };
    unsigned int phd$$$_p0_rde_fields [2];
    UINT32 phd$l_p0_flags;
    UINT32 phd$l_p0_region_prot;
    UINT64 phd$q_p0_region_id;
    void *phd$pq_p0_start_va;
    UINT64 phd$q_p0_region_size;
    union
    {
        void *phd$pq_p0_first_free_va;
        INT32 phd$l_frep0va;
    };
    union
    {
        INT64 phd$q_p1_rde;
        struct
        {
            struct _rde *phd$ps_p1_va_list_flink;
            struct _rde *phd$ps_p1_va_list_blink;
        };
    };
    unsigned int phd$$$_p1_rde_fields [2];
    UINT32 phd$l_p1_flags;
    UINT32 phd$l_p1_region_prot;
    UINT64 phd$q_p1_region_id;
    void *phd$pq_p1_start_va;
    UINT64 phd$q_p1_region_size;
    union
    {
        void *phd$pq_p1_first_free_va;
        INT32 phd$l_frep1va;
    };
    union
    {
        INT64 phd$q_p2_rde;
        struct
        {
            struct _rde *phd$ps_p2_va_list_flink;
            struct _rde *phd$ps_p2_va_list_blink;
        };
    };
    unsigned int phd$$$_p2_rde_fields [2];
    UINT32 phd$l_p2_flags;
    UINT32 phd$l_p2_region_prot;
    UINT64 phd$q_p2_region_id;
    void *phd$pq_p2_start_va;
    UINT64 phd$q_p2_region_size;
    void *phd$pq_p2_first_free_va;
    UINT64 phd$q_image_authpriv;
    UINT64 phd$q_image_permpriv;
    struct _rights *phd$ar_image_authrights;
    struct _rights *phd$ar_image_rights;
    struct _rights *phd$ar_subsystem_authrights;
    struct _rights *phd$ar_subsystem_rights;
    INT64 phd$q_bak_array;
};

#define PHD$S_PHDDEF 1016

#endif

