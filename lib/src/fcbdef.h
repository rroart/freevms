#ifndef FCBDEF_H
#define FCBDEF_H

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
    unsigned short int fcb$w_size;
    unsigned char fcb$b_type;
    unsigned char fcb$b_acclkmode;
    void *fcb$l_exfcb;
    struct _wcb *fcb$l_wlfl;
    struct _wcb *fcb$l_wlbl;
    unsigned int fcb$l_refcnt;
    unsigned int fcb$l_acnt;
    unsigned int fcb$l_wcnt;
    unsigned int fcb$l_lcnt;
    unsigned int fcb$l_tcnt;
    union
    {
        unsigned int fcb$l_status;
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
        unsigned short int fcb$w_fid [3];
        struct
        {
            unsigned short int fcb$w_fid_num;
            unsigned short int fcb$w_fid_seq;
            union
            {
                unsigned short int fcb$w_fid_rvn;
                struct
                {
                    unsigned char fcb$b_fid_rvn;
                    unsigned char fcb$b_fid_nmx;
                } fcb_fid_3;
            } fcb_fid_2;
        } fcb_fid_1_1;
        struct    // this is supposedly for 9660. can be used anyway
        {
            unsigned short int fcb$w_fid_dirnum;
            unsigned int fcb$l_fid_recnum;
        } fcb_fid_1_2;
    } fcb_fid_0;
    unsigned short int fcb$w_segn;
    unsigned int fcb$l_stvbn;
    unsigned int fcb$l_stlbn;
    unsigned int fcb$l_hdlbn;
    unsigned int fcb$l_filesize;
    unsigned int fcb$l_efblk;
    unsigned int fcb$l_versions;
    unsigned int fcb$l_dirseq;
    unsigned int fcb$l_dirindx;
    unsigned int fcb$l_acclkid;
    unsigned int fcb$l_lockbasis;
    union
    {
        unsigned int fcb$l_truncvbn;
        unsigned int fcb$l_numextents;
    };
    unsigned int fcb$l_cachelkid;
    unsigned int fcb$l_highwater;
    unsigned int fcb$l_newhighwater;
    unsigned int fcb$l_hwm_update;
    unsigned int fcb$l_hwm_erase;
    unsigned int fcb$l_hwm_partial;
    unsigned int fcb$l_revision;
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
            unsigned int fcb$l_fileowner;
            struct
            {
                unsigned short int fcb$w_uicmember;
                unsigned short int fcb$w_uicgroup;
            };
        };
        unsigned int fcb$l_fill_5;
        unsigned int fcb$l_fill_3;
        unsigned int fcb$l_fill_6;
        unsigned long long fcb$q_acmode;
        union
        {
            unsigned int fcb$l_sys_prot;
            struct
            {
                unsigned short int fcb$w_fileprot;
                unsigned short int fcb$w_fill_4;
            };
        };
        unsigned int fcb$l_own_prot;
        unsigned int fcb$l_grp_prot;
        unsigned int fcb$l_wor_prot;
        void *fcb$l_aclfl;
        void *fcb$l_aclbl;
        struct
        {
            char fcb$b_fill_1 [20];
        };
        struct
        {
            char fcb$b_fill_2 [20];
        };
        unsigned short int fcb$w_name_length;
        short int fcb$w_fill_7;
        void *fcb$l_name_pointer;
        struct _ocb *fcb$l_ocb;
        struct _orb *fcb$l_template_orb;
        unsigned int fcb$l_object_specific;
        struct _orb *fcb$l_original_orb;
        unsigned short int fcb$w_updseq;
        unsigned short int fcb$w_fill_8;
        unsigned int fcb$l_reserve1;
        unsigned int fcb$l_reserve2;
    };
    char fcb$b_fill_9 [4];
    union
    {
        char fcb$b_fill_10 [40];
        struct
        {
            char fcb$b_fill_11 [24];
            unsigned long long fcb$q_cfb;
            union
            {
                unsigned int fcb$l_caching_options;
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
                unsigned int fcb$l_status2;
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
    unsigned int fcb$l_dirlckid;
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
