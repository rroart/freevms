#ifndef IHPDEF_H
#define IHPDEF_H

#define IHP$K_LENGTH 44
#define IHP$C_LENGTH 44
#define IHP$S_IHPDEF 44

struct _ihp
{
    unsigned int ihp$l_eco1;
    unsigned int ihp$l_eco2;
    unsigned int ihp$l_eco3;
    unsigned int ihp$l_eco4;
    unsigned int ihp$l_rw_patsiz;
    void *ihp$l_rw_patadr;
    unsigned int ihp$l_ro_patsiz;
    void *ihp$l_ro_patadr;
    unsigned int ihp$l_patcomtxt;
    unsigned long long ihp$q_patdate;
};

#endif

