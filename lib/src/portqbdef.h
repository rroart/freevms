#ifndef portqbdef_h
#define portqbdef_h

struct _portqb
{
    unsigned long long portqb$r_cmdq_fill[8];
    unsigned long long portqb$q_rspq_fill;
    unsigned long portqb$l_dfqhdr;
    unsigned long portqb$l_mfqhdr;
    unsigned short portqb$w_dqelen;
    unsigned short portqb$w_dqelenhi;
    unsigned short portqb$w_mqelen;
    unsigned short portqb$w_mqelenhi;
    unsigned long portqb$l_vportqb;
    unsigned long portqb$l_vbdt;
    unsigned short portqb$_wbdtlen;
    unsigned short portqb$w_bdtlenhi;
    unsigned long portqb$l_sptbase;
    unsigned long portqb$l_sptlen;
    unsigned long portqb$l_gptbase;
    unsigned long portqb$l_gptlen;
};

#endif
