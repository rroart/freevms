#ifndef MMGDEF_H
#define MMGDEF_H

#define MMG$M_NO_MPL_FLUSH 4
#define MMG$V_NO_MPL_FLUSH 2
#define MMG$M_NOWAIT 2
#define MMG$V_NOWAIT 1
#define MMG$M_NOLASTUPD 1
#define MMG$V_NOLASTUPD 0
#define MMG$M_TICK_ENABLE 1
#define MMG$V_TICK_ENABLE 0
#define MMG$M_TROLL_ENABLE 2
#define MMG$V_TROLL_ENABLE 1
#define MMG$M_BOOTIME_MEMTEST 4
#define MMG$V_BOOTIME_MEMTEST 2
#define MMG$M_NO_MB 8
#define MMG$V_NO_MB 3
#define MMG$M_TICK_ACT 1
#define MMG$V_TICK_ACT 0

#define MMG$C_MPL_FLUSH_LIM 16

#define MMG$C_SECOBJNAM_SIZE 80

#define MMG$K_LENGTH -40
#define MMG$C_LENGTH -40
#define MMG$M_DELGBLDON 0x1
#define MMG$M_CHGPAGFIL 0x1
#define MMG$M_NOWAIT_IPL0 0x2
#define MMG$M_NO_OVERMAP 0x4
#define MMG$M_PARTIAL_FIRST 0x8
#define MMG$M_PARTIAL_LAST 0x10
#define MMG$M_NO_IRP_DELETE 0x20
#define MMG$M_DELPAG_NOP 0x40
#define MMG$M_CLUSTER_DEL 0x80
#define MMG$M_WINDOW 0x100
#define MMG$M_SHARED_L3PTS 0x200
#define MMG$M_RWAST_AT_IPL0 0x400
#define MMG$S_MMGDEF 41

struct _mmg
{
    unsigned long mmg$l_altsubr;
    unsigned long mmg$l_pgflcnt;
    unsigned long mmg$l_efblk;
    unsigned long mmg$r_bufobj_overlay;
    unsigned long mmg$l_vfyflags;
    unsigned long mmg$l_svstartva;
    unsigned long mmg$l_pagesubr;
    unsigned long mmg$l_savretadr;
    unsigned long mmg$l_calledipl;
    unsigned long mmg$r_per_page_overlay;
    unsigned long mmg$l_access_mode;
    unsigned long mmg$r_mmg_flags_overlay;
    struct
    {
        unsigned mmg$v_fill_1 : 1;
        unsigned mmg$v_chpagfil : 1;
        unsigned mmg$v_delgbldon : 1;
        unsigned mmg$v_nowait_ipl0 : 1;
        unsigned mmg$v_no_overmap : 1;
        unsigned mmg$v_jsb_altsubr : 1;
    };
    INT8 mmgdef$$_fill_2;
};

#define MMG$M_RES_MEM_ZERO 0x1
#define MMG$M_RES_MEM_GROUP 0x2
#define MMG$M_RES_MEM_PTS 0x4
#define MMG$M_RES_MEM_GBLSEC 0x8

struct _res_mem
{
    union
    {
        UINT32 mmg$l_res_mem_flags;
        struct
        {
            unsigned mmg$v_res_mem_zero : 1;
            unsigned mmg$v_res_mem_group : 1;
            unsigned mmg$v_res_mem_pts : 1;
            unsigned mmg$v_res_mem_gblsec : 1;
            unsigned mmg$v_fill_2_ : 4;
        };
    };
};

#define MMG$M_COLOR_MUST 0x1
#define MMG$M_COLOR_RANDOM 0x2
#define MMG$K_NO_VPN -1
#define MMG$K_NO_RAD -1
#define MMG$K_BASE_RAD -2

struct _page_color
{
    union
    {
        UINT32 mmg$l_page_color_flags;
        struct
        {
            unsigned mmg$v_color_must : 1;
            unsigned mmg$v_color_random : 1;
            unsigned mmg$v_fill_3_ : 6;
        };
    };
};

#define MMG$C_PRIVATE 1
#define MMG$C_SHARED 2
#define MMG$C_IO 3

#if defined(DoNotDefineThisQQ_)

#define MMG$K_POOLTYPE_NPP 0
#define MMG$K_POOLTYPE_BAP 1
#define MMG$K_POOLTYPE_MAXIMUM 2

#endif

typedef enum
{
    MMG$K_POOLTYPE_NPP=0,MMG$K_POOLTYPE_BAP,
    MMG$K_POOLTYPE_MAXIMUM
} MMG$POOL_TYPE;

#endif

