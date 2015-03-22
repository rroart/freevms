#ifndef TTYTADEF_H
#define TTYTADEF_H

#define TTY$K_TA_RCLLEN 256
#define TTY$S_TTYTADEF 284

struct _tt_type_ahd
{
    int (*tty$l_ta_put)(void);
    int (*tty$l_ta_get)(void);
    unsigned short int tty$w_ta_size;
    unsigned char tty$b_ta_type;
    unsigned char tty$b_ta_spare1;
    unsigned short int tty$w_ta_inahd;
    unsigned short int tty$w_ta_rcloff;
    void *tty$l_ta_end;
    unsigned short int tty$w_ta_rclsiz;
    unsigned short int tty$w_ta_spare2;
    char tty$a_ta_rcl [256];
    void *tty$l_ta_data;
};

#endif

