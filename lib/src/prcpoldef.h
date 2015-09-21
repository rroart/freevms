#ifndef PRCPOLDEF_H
#define PRCPOLDEF_H

#define PRCPOL$C_SIZ 56
#define PRCPOL$S_PRCPOLDEF 56

struct _prcpol
{
    UINT32 prcpol$l_sysidl;
    UINT16 prcpol$w_sysidh;
    UINT16 prcpol$w_fill_1;
    char prcpol$t_nodnam [16];
    UINT8 prcpol$b_prcnam [16];
    UINT8 prcpol$b_dirinf [16];
};

#endif

