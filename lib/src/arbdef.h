#ifndef arbdef_h
#define arbdef_h

#define ARB$C_HEADER 52
#define ARB$K_HEADER 52
#define ARB$K_LENGTH 124
#define ARB$C_LENGTH 124
#define ARB$S_ARBDEF 124

struct _arb
{
    unsigned long arb$l_priv;
    unsigned long arb$l_arb;
    unsigned char arb$t_res1[48];
    unsigned long arb$l_uic;
    unsigned char arb$t_res2[60];
};

#endif
