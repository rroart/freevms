#ifndef eihpdef_h
#define eihpdef_h

#define EIHP$K_LENGTH 60
#define EIHP$C_LENGTH 60
#define EIHP$S_EIHPDEF 60

struct _eihp
{
    struct
    {
        unsigned int eihp$l_majorid;
        unsigned int eihp$l_minorid;
    };
    unsigned int eihp$l_eco1;
    unsigned int eihp$l_eco2;
    unsigned int eihp$l_eco3;
    unsigned int eihp$l_eco4;
    unsigned int eihp$l_patcomtxt;
    unsigned int eihp$l_rw_patsiz;
    union
    {
        unsigned int eihp$l_rw_patadr;
        unsigned long long eihp$q_rw_patadr;
    };
    unsigned int eihp$l_ro_patsiz;
    union
    {
        unsigned int eihp$l_ro_patadr;
        unsigned long long eihp$q_ro_patadr;
    };
    unsigned long long eihp$q_patdate;
};

#endif

