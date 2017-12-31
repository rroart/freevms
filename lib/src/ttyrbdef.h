#ifndef ttyrbdef_h
#define ttyrbdef_h

#define TTY$M_RS_WRAP 0x1
#define TTY$S_TTYRBDEF 84

struct _tt_readbuf
{
    void *tty$l_rb_txt;
    void *tty$l_rb_uva;
    unsigned short int tty$w_rb_size;
    unsigned short int tty$w_rb_type;
    unsigned short int tty$w_rb_echlen;
    unsigned short int tty$w_rb_nonfill;
    unsigned long long tty$q_rb_echoarea;
    void *tty$l_rb_echstr;
    void *tty$l_rb_pic;
    void *tty$l_rb_term;
    unsigned int tty$l_rb_mod;
    void *tty$l_rb_aes;
    unsigned short int tty$w_rb_aeslen;
    union
    {
        unsigned short int tty$w_rb_rdstate;
        struct
        {
            unsigned tty$v_rs_wrap  : 1;
            unsigned tty$v_fill_11_ : 7;
        };
    };
    void *tty$l_rb_lin;
    unsigned short int tty$w_rb_linoff;
    unsigned short int tty$w_rb_linrest;
    unsigned short int tty$w_rb_prmlen;
    unsigned short int tty$w_rb_timos;
    unsigned short int tty$w_rb_cpzcur;
    unsigned short int tty$w_rb_cpzorg;
    unsigned short int tty$w_rb_txtoff;
    unsigned short int tty$w_rb_piclen;
    unsigned short int tty$w_rb_txtsiz;
    unsigned short int tty$w_rb_txtech;
    unsigned short int tty$w_rb_mode;
    unsigned char tty$b_rb_rvfclr;
    unsigned char tty$b_rb_rvffil;
    unsigned short int tty$w_rb_esctkn;
    unsigned short int tty$w_rb_txtonlysiz;
    union
    {
        void *tty$a_rb_prm;
        void *tty$l_rb_data;
    };
};

#endif

