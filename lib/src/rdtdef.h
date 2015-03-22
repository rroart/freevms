#ifndef RDTDEF_H
#define RDTDEF_H

#define RDT$S_RDTDEF 24

struct _rdt
{
    unsigned char rdt$b_scs_maint_block [16];
    void *rdt$l_waitfl;
    void *rdt$l_waitbl;
    unsigned short int rdt$w_size;
    unsigned char rdt$b_type;
    unsigned char rdt$b_subtyp;
    void *rdt$l_freerd;
    unsigned int rdt$l_maxrdidx;
    unsigned int rdt$l_qrdt_cnt;
};

#endif
