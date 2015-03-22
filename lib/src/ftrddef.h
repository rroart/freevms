#ifndef FTRDDEF_H
#define FTRDDEF_H

#define FTRD$K_LENGTH 40
#define FTRD$C_LENGTH 40
#define FTRD$S_FTRDDEF 40

struct _ftrd
{
    void *ftrd$l_astqfl;
    void *ftrd$l_astqbl;
    unsigned short int ftrd$w_size;
    unsigned char ftrd$b_type;
    unsigned char ftrd$b_rmod;
    unsigned int ftrd$l_pid;
    int (*ftrd$l_ast)(void);
    unsigned int ftrd$l_astprm;
    unsigned char ftrd$b_efn;
    unsigned char ftrd$b_unused;
    unsigned short int ftrd$w_read_size;
    void *ftrd$l_buff_addr;
    void *ftrd$l_char_addr;
    unsigned int ftrd$l_chars_read;
};

#endif

