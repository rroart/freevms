#ifndef EIHDDEF_H
#define EIHDDEF_H

#define EIHD$K_MAJORID 3
#define EIHD$K_MINORID 0
#define EIHD$K_MINORID_64 1
#define EIHD$K_EXE 1
#define EIHD$K_LIM 2
#define EIHD$C_MINCODE 0
#define EIHD$C_NATIVE 0
#define EIHD$C_CLI 1
#define EIHD$C_MAXCODE 1
#define EIHD$M_LNKDEBUG 0x1
#define EIHD$M_LNKNOTFR 0x2
#define EIHD$M_NOP0BUFS 0x4
#define EIHD$M_PICIMG 0x8
#define EIHD$M_P0IMAGE 0x10
#define EIHD$M_DBGDMT 0x20
#define EIHD$M_INISHR 0x40
#define EIHD$M_XLATED 0x80
#define EIHD$M_BIND_CODE_SEC 0x100
#define EIHD$M_BIND_DATA_SEC 0x200
#define EIHD$M_MKTHREADS 0x400
#define EIHD$M_UPCALLS 0x800
#define EIHD$M_OMV_READY 0x1000
#define EIHD$M_EXT_BIND_SECT 0x2000
#define EIHD$K_LENGTH 104
#define EIHD$C_LENGTH 104
#define EIHD$K_LENGTH_97 112
#define EIHD$K_ALIAS_MINCODE -1

#define EIHD$K_VAX -1
#define EIHD$K_RSX 0
#define EIHD$K_BPA 1
#define EIHD$K_ALIAS 2
#define EIHD$K_VAX_CLI 3
#define EIHD$K_PMAX 4

#define EIHD$K_ALPHA 5
#define EIHD$K_ALIAS_MAXCODE 5
#define EIHD$S_EIHDDEF 512

struct _eihd
{
    struct
    {
        unsigned int eihd$l_majorid;
        unsigned int eihd$l_minorid;
    };
    unsigned int eihd$l_size;
    unsigned int eihd$l_isdoff;
    unsigned int eihd$l_activoff;
    unsigned int eihd$l_symdbgoff;
    unsigned int eihd$l_imgidoff;
    unsigned int eihd$l_patchoff;
    union
    {
        void *eihd$l_iafva;
        long long eihd$q_iafva;
    };
    union
    {
        void *eihd$l_symvva;
        long long eihd$q_symvva;
    };
    unsigned int eihd$l_version_array_off;
    unsigned int eihd$l_imgtype;
    unsigned int eihd$l_subtype;
    unsigned int eihd$l_imgiocnt;
    unsigned int eihd$l_iochancnt;
    unsigned long long eihd$q_privreqs;
    unsigned int eihd$l_hdrblkcnt;
    union
    {
        unsigned int eihd$l_lnkflags;
        struct
        {
            unsigned eihd$v_lnkdebug        : 1;
            unsigned eihd$v_lnknotfr        : 1;
            unsigned eihd$v_nop0bufs        : 1;
            unsigned eihd$v_picimg      : 1;
            unsigned eihd$v_p0image     : 1;
            unsigned eihd$v_dbgdmt      : 1;
            unsigned eihd$v_inishr      : 1;
            unsigned eihd$v_xlated      : 1;
            unsigned eihd$v_bind_code_sec   : 1;
            unsigned eihd$v_bind_data_sec   : 1;
            unsigned eihd$v_mkthreads       : 1;
            unsigned eihd$v_upcalls     : 1;
            unsigned eihd$v_omv_ready       : 1;
            unsigned eihd$v_ext_bind_sect   : 1;
            unsigned eihd$v_fill_0_     : 2;
        };
    };
    unsigned int eihd$l_ident;
    unsigned int eihd$l_sysver;
    struct
    {
        unsigned char eihd$b_matchctl;
        unsigned char eihd$b_fill_1;
        unsigned char eihd$b_fill_2;
        unsigned char eihd$b_fill_3;
        unsigned int eihd$l_symvect_size;
        unsigned int eihd$l_virt_mem_block_size;
    };
    unsigned int eihd$l_ext_fixup_off;
    unsigned int eihd$l_noopt_psect_off;
    char eihd$t_skip [398];
    unsigned short int eihd$w_alias;
};

#endif

