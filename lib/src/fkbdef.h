#ifndef FKBDEF_H
#define FKBDEF_H

#define FKB$K_LENGTH 32
#define FKB$C_LENGTH 32
#define FKB$S_FKBDEF 32

struct _fkb
{
    struct _fkb *fkb$l_fqfl;
    struct _fkb *fkb$l_fqbl;
    UINT16 fkb$w_size;
    UINT8 fkb$b_type;
    UINT8 fkb$b_flck;
    void (*fkb$l_fpc)(void);
    unsigned long fkb$l_fr3;
    unsigned long fkb$l_fr4;
};

#endif

