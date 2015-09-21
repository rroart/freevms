#ifndef TTYRBDEF_H
#define TTYRBDEF_H

#define TTY$M_RS_WRAP 0x1
#define TTY$S_TTYRBDEF 84

struct _tt_readbuf
{
    void *tty$l_rb_txt;
    void *tty$l_rb_uva;
    UINT16 tty$w_rb_size;
    UINT16 tty$w_rb_type;
    UINT16 tty$w_rb_echlen;
    UINT16 tty$w_rb_nonfill;
    UINT64 tty$q_rb_echoarea;
    void *tty$l_rb_echstr;
    void *tty$l_rb_pic;
    void *tty$l_rb_term;
    UINT32 tty$l_rb_mod;
    void *tty$l_rb_aes;
    UINT16 tty$w_rb_aeslen;
    union
    {
        UINT16 tty$w_rb_rdstate;
        struct
        {
            unsigned tty$v_rs_wrap  : 1;
            unsigned tty$v_fill_11_ : 7;
        };
    };
    void *tty$l_rb_lin;
    UINT16 tty$w_rb_linoff;
    UINT16 tty$w_rb_linrest;
    UINT16 tty$w_rb_prmlen;
    UINT16 tty$w_rb_timos;
    UINT16 tty$w_rb_cpzcur;
    UINT16 tty$w_rb_cpzorg;
    UINT16 tty$w_rb_txtoff;
    UINT16 tty$w_rb_piclen;
    UINT16 tty$w_rb_txtsiz;
    UINT16 tty$w_rb_txtech;
    UINT16 tty$w_rb_mode;
    UINT8 tty$b_rb_rvfclr;
    UINT8 tty$b_rb_rvffil;
    UINT16 tty$w_rb_esctkn;
    UINT16 tty$w_rb_txtonlysiz;
    union
    {
        void *tty$a_rb_prm;
        void *tty$l_rb_data;
    };
};

#endif

