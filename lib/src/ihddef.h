#ifndef ihddef_h
#define ihddef_h

#define IHD$K_MAJORID 12848
#define IHD$K_MINORID 13616
#define IHD$K_EXE 1
#define IHD$K_LIM 2
#define IHD$M_LNKDEBUG 0x1
#define IHD$M_LNKNOTFR 0x2
#define IHD$M_NOP0BUFS 0x4
#define IHD$M_PICIMG 0x8
#define IHD$M_P0IMAGE 0x10
#define IHD$M_DBGDMT 0x20
#define IHD$M_INISHR 0x40
#define IHD$M_IHSLONG 0x80
#define IHD$M_UPCALLS 0x100
#define IHD$M_MATCHCTL 0x7000000
#define IHD$K_LENGTH 48
#define IHD$C_LENGTH 48
#define IHD$C_MINCODE -1
#define IHD$C_NATIVE -1
#define IHD$C_RSX 0
#define IHD$C_BPA 1
#define IHD$C_ALIAS 2
#define IHD$C_CLI 3
#define IHD$C_PMAX 4
#define IHD$C_ALPHA 5
#define IHD$C_MAXCODE 5

#define IHD$C_GEN_XLNKR 1
#define IHD$C_GEN_NATIVE 2
#define IHD$C_GEN_LNKFLG 3
#define IHD$C_GEN_SYSVER 4
#define IHD$C_GEN_FIXUP 5

#define IHD$C_GEN_NEWISD 6
#define IHD$S_IHDDEF 512

struct _ihd
{
    unsigned short int ihd$w_size;
    unsigned short int ihd$w_activoff;
    unsigned short int ihd$w_symdbgoff;
    unsigned short int ihd$w_imgidoff;
    unsigned short int ihd$w_patchoff;
    unsigned short int ihd$w_version_array_off;
    unsigned short int ihd$w_majorid;
    unsigned short int ihd$w_minorid;
    unsigned char ihd$b_hdrblkcnt;
    unsigned char ihd$b_imgtype;
    short int ihddef$$_fill_2;
    unsigned long long ihd$q_privreqs;
    unsigned short int ihd$w_iochancnt;
    unsigned short int ihd$w_imgiocnt;
    union
    {
        unsigned int ihd$l_lnkflags;
        struct
        {
            unsigned ihd$v_lnkdebug	: 1;
            unsigned ihd$v_lnknotfr	: 1;
            unsigned ihd$v_nop0bufs	: 1;
            unsigned ihd$v_picimg	: 1;
            unsigned ihd$v_p0image	: 1;
            unsigned ihd$v_dbgdmt	: 1;
            unsigned ihd$v_inishr	: 1;
            unsigned ihd$v_ihslong	: 1;
            unsigned ihd$v_upcalls	: 1;
            unsigned ihddef$$_fill_3	: 15;
            unsigned ihd$v_matchctl	: 3;
            unsigned ihd$v_fill_0_	: 5;
        };
    };
    unsigned int ihd$l_ident;
    unsigned int ihd$l_sysver;
    void *ihd$l_iafva;
    char ihd$t_skip [462];
    unsigned short int ihd$w_alias;
};

#endif

