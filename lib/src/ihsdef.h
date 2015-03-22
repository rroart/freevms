#ifndef IHSDEF_H
#define IHSDEF_H

#define IHS$K_LENGTH 28
#define IHS$C_LENGTH 28
#define IHS$S_IHSDEF 28

struct _ihs
{
    unsigned int ihs$l_dstvbn;
    unsigned int ihs$l_gstvbn;
    unsigned short int ihs$w_dstblks;
    unsigned short int ihs$w_gstrecs;
    unsigned int ihs$l_dmtvbn;
    unsigned int ihs$l_dmtbytes;
    int ihs$l_dstblks;
    int ihs$l_gstrecs;
};

#endif

