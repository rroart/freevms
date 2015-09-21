#ifndef TTYTADEF_H
#define TTYTADEF_H

#define TTY$K_TA_RCLLEN 256
#define TTY$S_TTYTADEF 284

struct _tt_type_ahd
{
    INT32 (*tty$l_ta_put)(void);
    INT32 (*tty$l_ta_get)(void);
    UINT16 tty$w_ta_size;
    UINT8 tty$b_ta_type;
    UINT8 tty$b_ta_spare1;
    UINT16 tty$w_ta_inahd;
    UINT16 tty$w_ta_rcloff;
    void *tty$l_ta_end;
    UINT16 tty$w_ta_rclsiz;
    UINT16 tty$w_ta_spare2;
    char tty$a_ta_rcl [256];
    void *tty$l_ta_data;
};

#endif

