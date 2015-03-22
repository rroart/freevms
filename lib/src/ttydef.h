#ifndef TTYDEF_H
#define TTYDEF_H

#define TTY$K_WB_LENGTH 56
#define TTY$C_WB_LENGTH 56
#define TTY$S_TTYWBDEF 60

struct _twp
{
    struct _twp *tty$l_wb_flink;
    struct _twp *tty$l_wb_blink;
    unsigned short int tty$w_wb_size;
    unsigned char tty$b_wb_type;
    unsigned char tty$b_wb_flck;
    void (*tty$l_wb_fpc)(void);
    unsigned long long tty$q_wb_fr3;
    unsigned long long tty$q_wb_fr4;
    void *tty$l_wb_map;
    void *tty$l_wb_next;
    void *tty$l_wb_end;
    struct _irp *tty$l_wb_irp;
    unsigned short int tty$w_wb_status;
    unsigned short int tty$w_wb_bcnt;
    int (*tty$l_wb_retaddr)(void);
    void *tty$l_wb_data;
};

#endif
