#ifndef FTRDDEF_H
#define FTRDDEF_H

#define FTRD$K_LENGTH 40
#define FTRD$C_LENGTH 40
#define FTRD$S_FTRDDEF 40

struct _ftrd
{
    void *ftrd$l_astqfl;
    void *ftrd$l_astqbl;
    UINT16 ftrd$w_size;
    UINT8 ftrd$b_type;
    UINT8 ftrd$b_rmod;
    UINT32 ftrd$l_pid;
    INT32 (*ftrd$l_ast)(void);
    UINT32 ftrd$l_astprm;
    UINT8 ftrd$b_efn;
    UINT8 ftrd$b_unused;
    UINT16 ftrd$w_read_size;
    void *ftrd$l_buff_addr;
    void *ftrd$l_char_addr;
    UINT32 ftrd$l_chars_read;
};

#endif

