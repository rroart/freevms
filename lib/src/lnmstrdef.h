#ifndef LNMSTRDEF_H
#define LNMSTRDEF_H

/* Author: Roar Thron�s */

#include <sysgen.h>
#include <linux/linkage.h>

#define LNM$M_NO_DELETE 0x10 /* for use in lnmb. the rest is decl otherplace */

#define LNMX$M_XEND 4

#define LNMTH$M_SHAREABLE   0x1
#define LNMTH$M_DIRECTORY   0x2
#define LNMTH$M_GROUP       0x4
#define LNMTH$M_SYSTEM      0x8
#define LNMTH$M_CLUSTERWIDE 0x10
#define LNMTH$M_REMACTION   0x20
#define LNMTH$K_LENGTH      40
#define LNMTH$S_LNMTHDEF    40

#define LNMX$M_CONCEALED    0x1
#define LNMX$M_TERMINAL     0x2
#define LNMX$C_HSHFCN       0x80
#define LNMX$C_BACKPTR      0x81
#define LNMX$C_TABLE        0x82
#define LNMX$C_IGNORED_INDEX 0x83
#define LNMX$C_CW_LINKS     0x84
#define LNMX$S_LNMXDEF      25

#define lnmmalloc(x) kmalloc(x,GFP_KERNEL);
#define lnmfree kfree
#undef lnmprintf
#define lnmprintf printk

struct _lnmth
{
    unsigned long lnmth$l_flags;
    void * lnmth$l_hash;
    struct orb * lnmth$l_orb;
    struct _lnmb * lnmth$l_name;
    void * lnmth$l_parent;
    void * lnmth$l_child;
    void * lnmth$l_sibling;
    void * lnmth$l_qtable;
    unsigned long lnmth$l_byteslm;
    unsigned long lnmth$l_bytes;
};

struct _lnmc
{
    struct _lnmc * lnmc$l_flink;
    struct _lnmc * lnmc$l_blink;
    unsigned short lnmc$w_size;
    unsigned char lnmc$b_type;
    unsigned char lnmc$b_mode;
    unsigned long lnmc$l_cacheindx;
    void * lnmc$l_tbladdr;
    unsigned long lnmc$l_procdirseq;
    unsigned long lnmc$l_sysdirseq;
    struct _lnmth * lnmc$l_entry[26]; /* unstandard */
};

struct _lnmx
{
    unsigned long lnmx$l_flags;
    unsigned long lnmx$l_index;
    unsigned long lnmx$l_hash;
    struct _lnmx *lnmx$l_next;
    unsigned int lnmx$l_pad;
    unsigned int lnmx$l_xlen;
    char lnmx$t_xlation[252];
};

struct _lnmb
{
    struct _lnmb * lnmb$l_flink;
    struct _lnmb * lnmb$l_blink;
    unsigned short lnmb$w_size;
    unsigned char lnmb$b_type;
    char empty;
    unsigned char lnmb$b_acmode;
    struct _lnmth * lnmb$l_table;
    struct _lnmx * lnmb$l_lnmx;

    unsigned char lnmb$b_flags; /* lnmdef is supposed to go here */
    unsigned char lnmb$b_count;
    unsigned char lnmb$t_name[254];
};

struct lnmhshp
{
    struct lnmhsh * lnmhsh$l_flink;
    struct lnmhsh * lnmhsh$l_blink;
    unsigned long lnmhsh$l_mask;
    unsigned long reserved;
    unsigned short lnmhsh$w_size;
    unsigned char lnmhsh$b_type;
    unsigned char reserved2;
    void * entry[2*LNMPHASHTBL];
};

struct lnmhshs
{
    struct lnmhsh * lnmhsh$l_flink;
    struct lnmhsh * lnmhsh$l_blink;
    unsigned long lnmhsh$l_mask;
    unsigned long reserved;
    unsigned short lnmhsh$w_size;
    unsigned char lnmhsh$b_type;
    unsigned char reserved2;
    void * entry[2*LNMSHASHTBL];
};

struct _lnmhsh
{
    unsigned int lnmhsh$l_mask;
    int lnmhsh$l_fill_1;
    unsigned short int lnmhsh$w_size;
    unsigned char lnmhsh$b_type;
    char lnmhsh$b_fill_2;
};

struct struct_nt
{
    /* ???? */
    struct _lnmb * lnmb;
    struct _lnmb * lnmb_cur;
    struct _lnmth * lnmth;
    char * lognam;
    unsigned long loglen;
    unsigned long hash;
};

struct struct_rt
{
    int flags;
    int depth;
    int tries;
    int acmode;
    struct _lnmc * cache;
    char * context[10];
};

struct struct_lnm_ret
{
    struct _lnmth * mylnmth;
    struct _lnmb * mylnmb;
};

#endif


