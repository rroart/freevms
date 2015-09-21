#ifndef PORTQBDEF_H
#define PORTQBDEF_H

#include <vms_types.h>

struct _portqb
{
    UINT64 portqb$r_cmdq_fill[8];
    UINT64 portqb$q_rspq_fill;
    unsigned long portqb$l_dfqhdr;
    unsigned long portqb$l_mfqhdr;
    UINT16 portqb$w_dqelen;
    UINT16 portqb$w_dqelenhi;
    UINT16 portqb$w_mqelen;
    UINT16 portqb$w_mqelenhi;
    unsigned long portqb$l_vportqb;
    unsigned long portqb$l_vbdt;
    UINT16 portqb$_wbdtlen;
    UINT16 portqb$w_bdtlenhi;
    unsigned long portqb$l_sptbase;
    unsigned long portqb$l_sptlen;
    unsigned long portqb$l_gptbase;
    unsigned long portqb$l_gptlen;
};

#endif
