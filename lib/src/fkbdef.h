#ifndef fkbdef_h
#define fkbdef_h

#define FKB$K_LENGTH 32
#define FKB$C_LENGTH 32
#define FKB$S_FKBDEF 32
 
struct _fkb {
    struct _fkb *fkb$l_fqfl;
    struct _fkb *fkb$l_fqbl;
    unsigned short fkb$w_size;
    unsigned char fkb$b_type;
    unsigned char fkb$b_flck;
    void (*fkb$l_fpc)(void);
    unsigned long fkb$q_fr3;
    unsigned long fkb$q_fr4;
    };

#endif
 
