#ifndef PTEDEF_H
#define PTEDEF_H

#define PTE$C_BYTES_PER_PTE 8
#define PTE$C_SHIFT_SIZE 3
#define PTE$M_VALID 0x1
#define PTE$M_FOR 0x2
#define PTE$M_FOW 0x4
#define PTE$M_FOE 0x8
#define PTE$M_ASM 0x10
#define PTE$M_GH 0x60
#define PTE$C_GROUP_OF_1 0
#define PTE$C_GROUP_OF_8 1
#define PTE$C_GROUP_OF_64 2
#define PTE$C_GROUP_OF_512 3
#define PTE$M_NO_MB 0x80
#define PTE$M_PROT 0xFF00
#define PTE$M_SOFTWARE 0xFFFF0000
#define PTE$M_PFN 0xFFFFFFFF00000000
#define PTE$M_KRE 0x100
#define PTE$M_ERE 0x200
#define PTE$M_SRE 0x400
#define PTE$M_URE 0x800
#define PTE$M_KWE 0x1000
#define PTE$M_EWE 0x2000
#define PTE$M_SWE 0x4000
#define PTE$M_UWE 0x8000
#define PTE$M_WINDOW 0x10000
#define PTE$M_OWN 0x60000
#define PTE$M_MODIFY 0x100000
#define PTE$M_CPY 0x30000000
#define PTE$M_NOX 0x40000000
#define PTE$M_S0_MBZ 0x80000000
#define PTE$M_TYP0 0x10000
#define PTE$M_PARTIAL_SECTION 0x80000
#define PTE$M_TYP1 0x100000
#define PTE$M_STX 0xFFFF00000000
#define PTE$M_CRF 0x1000000000000
#define PTE$M_DZRO 0x2000000000000
#define PTE$M_WRT 0x4000000000000
#define PTE$M_PGFLPAG 0xFFFFFF00000000
#define PTE$M_PGFLX 0xFF00000000000000
#define PTE$M_BAKX 0xFFFFFFFF00000000
#define PTE$M_GPTX 0xFFFFFFFF00000000
#define PTE$C_NOPGFLPAG 255
#define PTE$C_NA 0
#define PTE$C_KR 0x100
#define PTE$C_KW 0x1100
#define PTE$C_ER 0x300
#define PTE$C_EW 0x3300
#define PTE$C_SR 0x700
#define PTE$C_SW 0x7700
#define PTE$C_UR 0xf00
#define PTE$C_UW 0xff00
#define PTE$C_ERKW 0x1300
#define PTE$C_SRKW 0x1700
#define PTE$C_SREW 0x3700
#define PTE$C_URKW 0x1f00
#define PTE$C_UREW 0x3f00
#define PTE$C_URSW 0x7f00
#define PTE$C_KOWN 0
#define PTE$C_EOWN 0x20000
#define PTE$C_SOWN 0x40000
#define PTE$C_UOWN 0x60000
#define PTE$C_COPY 0
#define PTE$C_NOCOPY 0x10000000
#define PTE$C_DZRO 0x20000000
#define PTE$C_DZRO_L1PTE 0x1100
#define PTE$C_DZRO_L2PTE 0x40001300

#define PTE$S_PTEDEF 16

#define PTE$M_SINGLE_PTE 65536

#define PTE$C_INDEX_SHIFT_VALUE 19
#define PTE$C_FREE_BLOCK 16

#define PTELIST$K_LENGTH 24
#define PTELIST$C_LENGTH 24

struct _pte
{
    union
    {
        struct
        {
            unsigned pte$v_valid : 1;
            unsigned pte$v_for : 1;
            unsigned pte$v_fow : 1;
            unsigned pte$v_foe : 1;
            unsigned pte$v_asm : 1;
            unsigned pte$v_gh : 2;
            unsigned pte$v_no_mb : 1;
            unsigned pte$v_prot : 8;
            unsigned pte$v_software : 16;
            unsigned pte$v_pfn : 32;
        };
        struct
        {
            unsigned pte$v_filler_1 : 8;
            unsigned pte$v_kre : 1;
            unsigned pte$v_ere : 1;
            unsigned pte$v_sre : 1;
            unsigned pte$v_ure : 1;
            unsigned pte$v_kwe : 1;
            unsigned pte$v_ewe : 1;
            unsigned pte$v_swe : 1;
            unsigned pte$v_uwe : 1;
        };
        struct
        {
            unsigned pte$v_filler_2 : 16;
            unsigned pte$v_window : 1;
            unsigned pte$v_own : 2;
            unsigned pte$v_filler_9 : 1;
            unsigned pte$v_modify : 1;
            unsigned pte$v_filler_10 : 7;
            unsigned pte$v_cpy : 2;
            unsigned pte$v_nox : 1;
            unsigned pte$v_s0_mbz : 1;
        };
        struct
        {
            unsigned pte$v_filler_3 : 16;
            unsigned pte$v_typ0 : 1;
            unsigned pte$v_filler_4 : 2;
            unsigned pte$v_partial_section : 1;
            unsigned pte$v_typ1 : 1;
            unsigned pte$v_fill_0_ : 3;
        };
        union
        {
            struct
            {
                unsigned pte$v_filler_5 : 32;
                unsigned pte$v_stx : 16;
                unsigned pte$v_crf : 1;
                unsigned pte$v_dzro : 1;
                unsigned pte$v_wrt : 1;
                unsigned pte$v_fill_1_ : 5;
            };
            union
            {
                struct
                {
                    unsigned pte$v_filler_6 : 32;
                    unsigned pte$v_pgflpag : 24;
                    unsigned pte$v_pgflx : 8;
                };
                struct
                {
                    unsigned pte$v_filler_7 : 32;
                    unsigned pte$v_bakx : 32;
                };
            };
            struct
            {
                unsigned pte$v_filler_8 : 32;
                unsigned pte$v_gptx : 32;
            };
        };
    };
};

struct _va_pte_free
{
    unsigned long long pte$q_index;

    int pte$$_filler_14;
    unsigned int pte$l_free_count;
};

struct _ptelist
{
    unsigned long long ptelist$q_head;
    unsigned long long ptelist$q_tail;
    long long ptelist$q_count;
};

typedef struct _pte * PTE_PQ;
typedef struct _pte ** PTE_PPQ;
typedef struct _ptelist * PTELIST_PQ;
typedef struct _ptelist ** PTELIST_PPQ;
typedef struct _va_pte_free * VA_PTE_FREE_PQ;
typedef struct _va_pte_free ** VA_PTE_FREE_PPQ;

#if 0
typedef unsigned long long PTE_PQ;
typedef unsigned long long PTE_PPQ;
typedef unsigned long long PTELIST_PQ;
typedef unsigned long long PTELIST_PPQ;
typedef unsigned long long VA_PTE_FREE_PQ;
typedef unsigned long long VA_PTE_FREE_PPQ;
#endif

#endif

