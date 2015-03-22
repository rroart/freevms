#ifndef RVTDEF_H
#define RVTDEF_H

#define     RVT$K_LENGTH        88
#define     RVT$C_LENGTH        88
#define     RVT$C_UCB_POINTER   0
#define     RVT$C_PHYSICAL_VOLUME   1
#define     RVT$C_VOLUME_LOCK_ID    2
#define     RVT$C_VOLUME_IDENTIFIER 3

#define     RVT$C_RVTVCB        4
#define     RVT$C_MINSIZE       18
#define     RVT$S_RVTDEF        88

struct _rvt
{
    unsigned int rvt$l_struclkid;
    unsigned int rvt$l_refc;
    unsigned short int rvt$w_size;
    unsigned char rvt$b_type;
    unsigned char rvt$b_nvols;
    char rvt$t_strucname [12];
    char rvt$t_vlslcknam [12];
    unsigned int rvt$l_blockid;
    unsigned char rvt$b_acb [36];
    unsigned int rvt$l_trans;
    unsigned int rvt$l_activity;
    union
    {
        struct _ucb *rvt$l_ucblst;
        void *rvt$a_rvtvcb;
    };
};

#endif

