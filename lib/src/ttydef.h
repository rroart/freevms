#ifndef TTYDEF_H
#define TTYDEF_H

#include <vms_types.h>

#define TTY$K_WB_LENGTH 56
#define TTY$C_WB_LENGTH 56
#define TTY$S_TTYWBDEF 60

struct _twp
{
    struct _twp *tty$l_wb_flink;
    struct _twp *tty$l_wb_blink;
    UINT16 tty$w_wb_size;
    UINT8 tty$b_wb_type;
    UINT8 tty$b_wb_flck;
    void (*tty$l_wb_fpc)(void);
    UINT64 tty$q_wb_fr3;
    UINT64 tty$q_wb_fr4;
    void *tty$l_wb_map;
    void *tty$l_wb_next;
    void *tty$l_wb_end;
    struct _irp *tty$l_wb_irp;
    UINT16 tty$w_wb_status;
    UINT16 tty$w_wb_bcnt;
    INT32 (*tty$l_wb_retaddr)(void);
    void *tty$l_wb_data;
};

#endif
