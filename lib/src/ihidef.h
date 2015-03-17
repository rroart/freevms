#ifndef ihidef_h
#define ihidef_h

#define IHI$K_LENGTH 80
#define IHI$C_LENGTH 80
#define IHI$S_IHIDEF 80

struct _ihi
{
    char ihi$t_imgnam [40];
    char ihi$t_imgid [16];
    unsigned long long ihi$q_linktime;
    char ihi$t_linkid [16];
};

#endif

