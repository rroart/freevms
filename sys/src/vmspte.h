#ifndef VMSPTE_H
#define VMSPTE_H

// $Id$
// $Locker$

// Author. Roar Thronæs.

// This will replace lib/src/ptedef.h, which is for Alpha
// Later, this will replace include/asm/pgtable.h

//FreeVMS PTE additions
#define _PAGE_BIT_TYP1 11
#define _PAGE_BIT_TYP0 10
#define _PAGE_BIT_TYP 9
#define _PAGE_TYP1 0x800
#define PTE$M_TYP1 0x800
#define _PAGE_TYP0 0x400
#define PTE$M_TYP0 0x400
//#define _PAGE_TYP 0x200


#define PTE$M_CRF 0x1000

struct _mypte
{
    union
    {
        struct
        {
            unsigned page_bits : 9; //refer to pgtable.h for these
            unsigned yet_unused : 1;
            unsigned pte$v_typ0 : 1;
            unsigned pte$v_typ1 : 1;
            unsigned pte$v_stx : 16;
            unsigned pte$v_crf : 1;
            unsigned pte$v_dzro : 1;
            unsigned pte$v_wrt : 1;
            unsigned pte$v_fill : 1;
        };
        struct
        {
            unsigned page_bits1 : 9; //refer to pgtable.h for these
            unsigned yet_unused1 : 1;
            unsigned pte$v_typ01 : 1;
            unsigned pte$v_typ11 : 1;
            unsigned pte$v_pgflpag : 18;
            unsigned pte$v_pgflx : 2;
        };
        struct
        {
            unsigned page_bits2 : 9; //refer to pgtable.h for these
            unsigned yet_unused2 : 1;
            unsigned pte$v_typ02 : 1;
            unsigned pte$v_typ12 : 1;
            unsigned pte$v_gptx : 20;
        };
        struct
        {
            unsigned page_bits3 : 9; //refer to pgtable.h for these
            unsigned yet_unused3 : 1;
            unsigned pte$v_typ03 : 1;
            unsigned pte$v_typ13 : 1;
            unsigned pte$v_pfn : 20;
        };
        struct
        {
            unsigned page_bits4 : 9; //refer to pgtable.h for these
            unsigned pte$v_window : 1;
            unsigned pte$v_typ04 : 1;
            unsigned pte$v_typ14 : 1;
            unsigned pte$v_pfn1 : 20;
        };
        struct
        {
            unsigned pte$v_valid : 1;
            unsigned nothing : 6;
            unsigned pte$v_global : 1;
            unsigned pte$v_gblwrt : 1;
            unsigned pte$v_fill1 : 23;
        };
        unsigned long pte$l_all;
    };
};

#endif

