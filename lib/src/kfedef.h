#ifndef KFEDEF_H
#define kfedef_H

#define     KFE$M_PROTECT       0x1
#define     KFE$M_LIM       0x2
#define     KFE$M_PROCPRIV      0x4
#define     KFE$M_OPEN      0x8
#define     KFE$M_HDRRES        0x10
#define     KFE$M_SHARED        0x20
#define     KFE$M_KFE_NAMING    0x40
#define     KFE$M_COMPRESS      0x80
#define     KFE$M_NOPURGE       0x100
#define     KFE$M_ACCOUNT       0x200
#define     KFE$M_WRITEABLE     0x400
#define     KFE$M_EXEONLY       0x800
#define     KFE$M_DISCONTIGUOUS 0x1000
#define     KFE$M_DELETE_PEND   0x2000
#define     KFE$M_VERSION_SAFE  0x4000
#define     KFE$M_DATA_RESIDENT 0x8000
#define     KFE$M_AUTHPRIV      0x1
#define     KFE$M_ARB_SUPPORT   0x2
#define     KFE$K_LENGTH        112
#define     KFE$C_LENGTH        112

#define     KFE$K_NUMBER_OF_FLAGS   18
#define     KFE$S_KFEDEF        112

struct _kfe
{
    void *kfe$l_hshlnk;
    struct _kfe *kfe$l_kfelink;
    unsigned short int kfe$w_size;
    unsigned char kfe$b_type;
    unsigned char kfe$b_hshidx;
    struct _kfd *kfe$l_kfd;
    union
    {
        unsigned short int kfe$w_flags;
        struct
        {
            unsigned kfe$v_protect      : 1;
            unsigned kfe$v_lim      : 1;
            unsigned kfe$v_procpriv     : 1;
            unsigned kfe$v_open     : 1;
            unsigned kfe$v_hdrres       : 1;
            unsigned kfe$v_shared       : 1;
            unsigned kfe$v_kfe_naming       : 1;
            unsigned kfe$v_compress     : 1;
            unsigned kfe$v_nopurge      : 1;
            unsigned kfe$v_account      : 1;
            unsigned kfe$v_writeable        : 1;
            unsigned kfe$v_exeonly      : 1;
            unsigned kfe$v_discontiguous    : 1;
            unsigned kfe$v_delete_pend  : 1;
            unsigned kfe$v_version_safe : 1;
            unsigned kfe$v_data_resident    : 1;
        };
    };
    unsigned short int kfe$w_gblseccnt;
    unsigned int kfe$l_usecnt;
    union
    {
        struct _wcb *kfe$l_wcb;
        struct
        {
            union
            {
                unsigned short int kfe$w_fid;
                unsigned short int kfe$w_fid_num;
            };
            unsigned short int kfe$w_fid_seq;
        };
    };
    union
    {
        struct _ihd *kfe$l_imghdr;
        unsigned short int kfe$w_fid_rvn;
    };
    unsigned long long kfe$q_procpriv;
    unsigned char kfe$b_matchctl;
    char kfedef$$_fill_4;
    unsigned short int kfe$w_amecod;
    unsigned int kfe$l_ident;
    struct _orb *kfe$l_orb;
    unsigned short int kfe$w_shrcnt;
    unsigned short int kfe$w_maxshrisd;
    struct _kferes *kfe$l_kferes_ptr;
    unsigned int kfe$l_ref_count;
    unsigned int kfe$l_priv_isd_cnt;
    unsigned int kfe$l_image_size;
    unsigned int kfe$l_obsolete_1;
    unsigned int kfe$l_imagename_offset;
    union
    {
        unsigned int kfe$l_flags2;
        struct
        {
            unsigned kfe$v_authpriv     : 1;
            unsigned kfe$v_arb_support  : 1;
            unsigned kfe$v_fill_0_      : 6;
        };
    };
    void *kfe$ar_authrights;
    void *kfe$ar_rights;
    unsigned int kfe$l_arb_support;
    unsigned long long kfe$q_authpriv;
    unsigned int kfe$l_risig_offset;
    unsigned short int kfe$w_filver;
    short int kfedef$$_fill_5;
};

#endif

