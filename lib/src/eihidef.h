#ifndef EIHIDEF_H
#define EIHIDEF_H

#define EIHI$K_MAJORID 1
#define EIHI$K_MINORID 2
#define EIHI$K_LENGTH 104
#define EIHI$C_LENGTH 104
#define EIHI$S_EIHIDEF 104

struct _eihi
{
    struct
    {
        unsigned int eihi$l_majorid;
        unsigned int eihi$l_minorid;
    };
    unsigned long long eihi$q_linktime;
    char eihi$t_imgnam [40];
    char eihi$t_imgid [16];
    char eihi$t_linkid [16];
    char eihi$t_imgbid [16];
};

#endif

