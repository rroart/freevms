#ifndef lnm_h
#define lnm_h

/* Author: Roar Thronæs */

#include"../../sys/src/sysgen.h"
#include<linux/linkage.h>

#define LNM$M_NO_DELETE 0x10 /* for use in lnmb. the rest is decl otherplace */

#define LNM$M_MYCONCEALED 1 /* for use with lnmx */
#define LNM$M_MYTERMINAL 2
#define LNM$M_MYXEND 4

#define LNM$M_MYSHAREABLE 1 /* for use with lnmth */
#define LNM$M_MYDIRECTORY 2
#define LNM$M_MYGROUP 4
#define LNM$M_MYSYSTEM 8

#define LNM$C_BACKPOINTER 0x81
#define LNM$C_TABLE 0x82	/* for use with lnmx index */

#ifndef USERLAND
#define lnmmalloc kmalloc
#define lnmfree kfree
#else
#define lnmmalloc malloc
#define lnmfree free
#endif

struct lnmth {
unsigned char lnmth$b_flags;
unsigned char none[3];
/*unsigned long*/ void * lnmth$l_hash;
/*unsigned long*/ struct orb * lnmth$l_orb;
/*unsigned long*/ struct lnmb * lnmth$l_name;
/*unsigned long*/ void * lnmth$l_parent;
/*unsigned long*/ void * lnmth$l_child;
/*unsigned long*/ void * lnmth$l_sibling;
/*unsigned long*/ void * lnmth$l_qtable;
unsigned long lnmth$l_byteslm;
unsigned long lnmth$l_bytes;
};

struct lnmc {
struct lnmc * lnmc$l_flink;
struct lnmc * lnmc$l_blink;
unsigned short lnmc$w_size;
unsigned char lnmc$b_type;
unsigned char lnmc$b_cacheindx;
/*unsigned long*/ void * lnmc$l_tbladdr;
unsigned long lnmc$l_procdirseq;
unsigned long lnmc$l_sysdirseq;
struct lnmth lnmths[26]; /* unstandard */
};

struct lnmx {
unsigned char lnmx$b_flags;
unsigned char lnmx$b_index;
unsigned short lnmx$b_hash;
  unsigned short lnmx$b_count;
/*unsigned*/ char lnmx$t_xlation[254];
unsigned char none;
};

struct lnmb {
struct lnmb * lnmb$l_flink;
struct lnmb * lnmb$l_blink;
unsigned short lnmb$w_size;
unsigned char lnmb$b_type;
unsigned char lnmb$b_acmode;
/*unsigned long*/ struct lnmth * lnmb$l_table; 
unsigned char lnmb$b_flags; /* lnmdef is supposed to go here */
unsigned char lnmb$b_count;
unsigned char lnmb$t_name[254];
struct lnmx lnmxs[20]; /* unstandard, and at what size? */
unsigned char four;
};

struct lnmhshp {
struct lnmhsh * lnmhsh$l_flink;
struct lnmhsh * lnmhsh$l_blink;
unsigned long lnmhsh$l_mask;
unsigned long reserved;
unsigned short lnmhsh$w_size;
unsigned char lnmhsh$b_type;
unsigned char reserved2;
struct lnmb entry[LNMPHASHTBL];
};

struct lnmhshs {
struct lnmhsh * lnmhsh$l_flink;
struct lnmhsh * lnmhsh$l_blink;
unsigned long lnmhsh$l_mask;
unsigned long reserved;
unsigned short lnmhsh$w_size;
unsigned char lnmhsh$b_type;
unsigned char reserved2;
void * entry[2*LNMSHASHTBL];
};

struct struct_nt {
/* ???? */
int depth;
int tries;
int acmode;
struct lnmc * cache;
char * context[10];
};

struct struct_rt {
int depth;
int tries;
int acmode;
struct lnmc * cache;
char * context[10];
};

#endif
