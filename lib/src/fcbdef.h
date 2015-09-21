#ifndef FCBDEF_H
#define FCBDEF_H

#include <vms_types.h>

#define     FCB$M_FILE_ATTRIBUTES       0xF
#define     FCB$M_FILE_CONTENTS     0xF0
#define     FCB$M_FLUSH_ON_CLOSE        0xF00
#define     FCB$M_CACHING_OPTIONS_MBZ   0xFFFFF000
#define     FCB$C_DEFAULT       0
#define     FCB$C_WRITETHROUGH  1
#define     FCB$C_WRITEBEHIND   2
#define     FCB$C_NOCACHING     3
#define     FCB$C_FLUSH     1
#define     FCB$C_NOFLUSH       2
#define     FCB$M_VCC_STATE     0x7
#define     FCB$K_LENGTH        320
#define     FCB$C_LENGTH        320
#define     FCB$S_FCBDEF        320

struct _fcb
{
    struct _fcb *fcb$l_fcbfl;
    struct _fcb *fcb$l_fcbbl;
    UINT16 fcb$w_size;
    UINT8 fcb$b_type;
    UINT8 fcb$b_acclkmode;
    void *fcb$l_exfcb;
    struct _wcb *fcb$l_wlfl;
    struct _wcb *fcb$l_wlbl;
    UINT32 fcb$l_refcnt;
    UINT32 fcb$l_acnt;
    UINT32 fcb$l_wcnt;
    UINT32 fcb$l_lcnt;
    UINT32 fcb$l_tcnt;
    union
    {
        UINT32 fcb$l_status;
        struct
        {
            unsigned fcb$v_dir : 1;
            unsigned fcb$v_markdel : 1;
            unsigned fcb$v_badblk : 1;
            unsigned fcb$v_excl : 1;
            unsigned fcb$v_spool : 1;
            unsigned fcb$v_rmslock : 1;
            unsigned fcb$v_erase : 1;
            unsigned fcb$v_badacl : 1;
            unsigned fcb$v_stale : 1;
            unsigned fcb$v_delaytrnc : 1;
            unsigned fcb$v_limbo : 1;
            unsigned fcb$v_isdir : 1;
            unsigned fcb$v_nomove : 1;
            unsigned fcb$v_shelved : 1;
            unsigned fcb$v_noshelvable : 1;
            unsigned fcb$v_preshelved : 1;
        };
    };
    union
    {
        UINT16 fcb$w_fid [3];
        struct
        {
            UINT16 fcb$w_fid_num;
            UINT16 fcb$w_fid_seq;
            union
            {
                UINT16 fcb$w_fid_rvn;
                struct
                {
                    UINT8 fcb$b_fid_rvn;
                    UINT8 fcb$b_fid_nmx;
                } fcb_fid_3;
            } fcb_fid_2;
        } fcb_fid_1_1;
        struct    // this is supposedly for 9660. can be used anyway
        {
            UINT16 fcb$w_fid_dirnum;
            UINT32 fcb$l_fid_recnum;
        } fcb_fid_1_2;
    } fcb_fid_0;
    UINT16 fcb$w_segn;
    UINT32 fcb$l_stvbn;
    UINT32 fcb$l_stlbn;
    UINT32 fcb$l_hdlbn;
    UINT32 fcb$l_filesize;
    UINT32 fcb$l_efblk;
    UINT32 fcb$l_versions;
    UINT32 fcb$l_dirseq;
    UINT32 fcb$l_dirindx;
    UINT32 fcb$l_acclkid;
    UINT32 fcb$l_lockbasis;
    union
    {
        UINT32 fcb$l_truncvbn;
        UINT32 fcb$l_numextents;
    };
    UINT32 fcb$l_cachelkid;
    UINT32 fcb$l_highwater;
    UINT32 fcb$l_newhighwater;
    UINT32 fcb$l_hwm_update;
    UINT32 fcb$l_hwm_erase;
    UINT32 fcb$l_hwm_partial;
    UINT32 fcb$l_revision;
    union
    {
        struct
        {
            void *fcb$l_hwm_waitfl;
            void *fcb$l_hwm_waitbl;
        };
        struct
        {
            void *fcb$l_limbofl;
            void *fcb$l_limbobl;
        };
    };
    struct
    {
        union
        {
            UINT32 fcb$l_fileowner;
            struct
            {
                UINT16 fcb$w_uicmember;
                UINT16 fcb$w_uicgroup;
            };
        };
        UINT32 fcb$l_fill_5;
        UINT32 fcb$l_fill_3;
        UINT32 fcb$l_fill_6;
        UINT64 fcb$q_acmode;
        union
        {
            UINT32 fcb$l_sys_prot;
            struct
            {
                UINT16 fcb$w_fileprot;
                UINT16 fcb$w_fill_4;
            };
        };
        UINT32 fcb$l_own_prot;
        UINT32 fcb$l_grp_prot;
        UINT32 fcb$l_wor_prot;
        void *fcb$l_aclfl;
        void *fcb$l_aclbl;
        struct
        {
            INT8 fcb$b_fill_1 [20];
        };
        struct
        {
            INT8 fcb$b_fill_2 [20];
        };
        UINT16 fcb$w_name_length;
        INT16 fcb$w_fill_7;
        void *fcb$l_name_pointer;
        struct _ocb *fcb$l_ocb;
        struct _orb *fcb$l_template_orb;
        UINT32 fcb$l_object_specific;
        struct _orb *fcb$l_original_orb;
        UINT16 fcb$w_updseq;
        UINT16 fcb$w_fill_8;
        UINT32 fcb$l_reserve1;
        UINT32 fcb$l_reserve2;
    };
    INT8 fcb$b_fill_9 [4];
    union
    {
        INT8 fcb$b_fill_10 [40];
        struct
        {
            INT8 fcb$b_fill_11 [24];
            UINT64 fcb$q_cfb;
            union
            {
                UINT32 fcb$l_caching_options;
                struct
                {
                    unsigned fcb$v_file_attributes : 4;
                    unsigned fcb$v_file_contents : 4;
                    unsigned fcb$v_flush_on_close : 4;
                    unsigned fcb$v_caching_options_mbz : 20;
                };
            };
            union
            {
                UINT32 fcb$l_status2;
                struct
                {
                    unsigned fcb$v_vcc_state : 3;
                    unsigned fcb$v_fill_4_ : 5;
                };
            };
        };
    };
    struct _orb *fcb$l_orb;
    void *fcb$l_cfcb;
    struct _fcb *fcb$l_primfcb;
    UINT32 fcb$l_dirlckid;
};

#define fcb$w_fid fcb_fid_0.fcb$w_fid
#define fcb$w_fid_num fcb_fid_0.fcb_fid_1_1.fcb$w_fid_num
#define fcb$w_fid_seq fcb_fid_0.fcb_fid_1_1.fcb$w_fid_seq
#define fcb$w_fid_rvn fcb_fid_0.fcb_fid_1_1.fcb_fid_2.fcb$w_fid_rvn
#define fcb$b_fid_rvn fcb_fid_0.fcb_fid_1_1.fcb_fid_2.fcb_fid_3.fcb$b_fid_rvn
#define fcb$b_fid_nmx fcb_fid_0.fcb_fid_1_1.fcb_fid_2.fcb_fid_3.fcb$b_fid_nmx
#define fcb$w_fid_dirnum fcb_fid_0.fcb_fid_1_2.fcb$w_fid_dirnum
#define fcb$l_fid_recnum fcb_fid_0.fcb_fid_1_2.fcb$l_fid_recnum

#endif
