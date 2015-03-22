#ifndef CEBDEF_H
#define CEBDEF_H

#define CEB$K_LENGTH 72
#define CEB$C_LENGTH 72
#define CEB$S_CEBDEF 72

struct _ceb
{
    struct _ceb *ceb$l_cebfl;
    struct _ceb *ceb$l_cebbl;
    unsigned short int ceb$w_size;
    unsigned char ceb$b_type;
    char ceb$b_subtype;
    union
    {
        unsigned int ceb$l_sts;
        struct
        {
            unsigned ceb$v_noquota : 1;
            unsigned ceb$v_perm : 1;
            unsigned ceb$v_fill_0_ : 6;
        };
    };
    unsigned int ceb$l_pid;
    unsigned int ceb$l_efc;
    void *ceb$l_wqfl;
    void *ceb$l_wqbl;
    unsigned int ceb$l_wqcnt;
    unsigned int ceb$l_state;
    struct _orb *ceb$l_orb;
    union
    {
        unsigned int ceb$l_uic;
        struct
        {
            char cebdef$$_fill_2 [2];
            unsigned short int ceb$w_grp;
        };
    };
    unsigned int ceb$l_prot;
    unsigned int ceb$l_refc;
    char ceb$t_efcnam [16];
};

#endif

