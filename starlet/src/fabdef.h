#ifndef fabdef_h
#define fabdef_h

/* RMS.h v1.3   RMS routine definitions */

/*
        This is part of ODS2 written by Paul Nankervis,
        email address:  Paulnank@au1.ibm.com

        ODS2 is distributed freely for all members of the
        VMS community to use. However all derived works
        must maintain comments in their source to acknowledge
        the contibution of the original author.

	Originally part of rms.h
*/

#define FAB$M_NAM 0x1000000

#define FAB$C_SEQ 0
#define FAB$C_REL 16
#define FAB$C_IDX 32
#define FAB$C_HSH 48

#define FAB$M_FTN 1
#define FAB$M_CR  2
#define FAB$M_PRN 4
#define FAB$M_BLK 8

#define FAB$M_PUT 0x1
#define FAB$M_GET 0x2
#define FAB$M_DEL 0x4
#define FAB$M_UPD 0x8
#define FAB$M_TRN 0x10
#define FAB$M_BIO 0x20
#define FAB$M_BRO 0x40
#define FAB$M_EXE 0x80

#define FAB$C_UDF 0
#define FAB$C_FIX 1
#define FAB$C_VAR 2
#define FAB$C_VFC 3
#define FAB$C_STM 4
#define FAB$C_STMLF 5
#define FAB$C_STMCR 6

struct _fabdef {
    struct _namdef *fab$l_nam;
    int fab$w_ifi;
    char *fab$l_fna;
    char *fab$l_dna;
    int fab$b_fns;
    int fab$b_dns;
    int fab$l_alq;
    int fab$b_bks;
    int fab$w_deq;
    int fab$b_fsz;
    int fab$w_gbc;
    int fab$w_mrs;
    int fab$l_fop;
    int fab$b_org;
    int fab$b_rat;
    int fab$b_rfm;
    int fab$b_fac;
    void *fab$l_xab;
};

#endif
